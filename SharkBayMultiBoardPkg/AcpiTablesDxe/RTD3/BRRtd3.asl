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
;*    Intel Corporation - ACPI Reference Code for the Sandy Bridge        *;
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

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    1,
    "AcpiRef",
    "BR0_Rtd3",
    0x1000
    )
{
#define BRRTD3 1
Include("RTD3Common.asl")
#undef BRRTD3

External(BBR, IntObj)         // BaskinRidge
External(BWC, IntObj)         // Walnut Canyon
External(\_SB.PCI0.EHC2.HUBN.PR01.PR11, DeviceObj)
External(\_SB.PCI0.EHC2.HUBN.PR01.PR12, DeviceObj)
External(\_SB.PCI0.LPCB.GR19)
External(\_SB.PCI0.LPCB.GR0B)
External(\RT14)
External(\RT16)
External(\RT17)
External(\RT32)
External(\RT35)
External(\GL01) 
External(\GIV1)
External(\GL00)
External(\GL04)
External(\GL05)
External(\GL06)
External(\GL08)
External(\SS11)
External(\SE11)
External(\SS13)
External(\SE13)
External(PFLV)
External(FDTP)


If(LOr(LAnd(LEqual(BID,BBR),LEqual(RTD3,1)),LAnd(LEqual(BID,BWC),LEqual(RTD3,1)))) {


//
// PCIe RTD3 - for slot#3
//

Scope (\_SB.PCI0.RP03)
{
  Name(WC, 0)
}

Scope (\_SB.PCI0.RP03)
{
  Name(_PR0, Package(){PXP3})
  Name(_PR3, Package(){PXP3})
  Name(WKEN, 0)

  Method(_S0W, 0)
  { // PMEs can be generated from D3cold
    If(And(\_SB.OSCO, 0x04))
    { // OS comprehends D3cold, as described via \_SB._OSC
      Return(4)
    } Else {
      Return(3)
    }
  } // End _S0W

  Method(_DSW, 3)
  {
    If(Arg1)
    { // Entering Sx, need to disable WAKE# from generating runtime PME
        Store(0, WKEN)
    } Else {  // Staying in S0
      If(LAnd(Arg0, Arg2)) // Exiting D0 and arming for wake
      { // Set PME
        Store(1, WKEN)
      } Else { // Disable runtime PME, either because staying in D0 or disabling wake 
        Store(0, WKEN)
      }
    }
  } // End _DSW


 // Define a power resource for PCIe RP3 D0-D3hot
  PowerResource(PXP3, 0, 0)
  {
    Name(OFFC, 0)     // _OFF Counter
    Name(ONC, 0)      // _ON Counter
    Name(PWRS, 0xFF)  // current power status

    Method(_STA, 0)
    {
      If(LAnd(NAnd(GL00, 0x40), And(GL06, 0x80))){
        Return (1)
      } Else {
        Return (0)
      }

    } 
    Method(_ON) // Turn on core power to PCIe Slot3
    {
      Increment(ONC)
      //Power ON for Slot3
      And(\GL00, 0x0BF, \GL00)
      Sleep(PEP0)     // Sleep for programmable delay
      Store(1, PWRS)
      Or(\GL06, 0x80, \GL06)
    }

    Method(_OFF) // Turn off core power to PCIe Slot1
    {
      Increment(OFFC)
      //Power OFF for Slot3
      And(\GL06, 0x7F, \GL06)
      Or(\GL00, 0x40, \GL00)
      Store(1, \_SB.PCI0.RP03.LDIS) //Not required as power not removed at this time
      Store(0, \_SB.PCI0.RP03.LDIS) //toggle link disable

      If(\_SB.PCI0.RP03.WKEN) {
          Store(0, PWRS)
      }
    }
  } // End PXP3
} // End RP03
/*
Scope(\_SB.PCI0.RP03.PXSX)
{ // PCIe Device beneath RP03
  Method(_S0W, 0)
  { // PMEs can be generated from D3cold
    If(And(\_SB.OSCO, 0x04))
    { // OS comprehends D3cold, as described via \_SB._OSC
      Return(4)
    } Else {
      Return(3)
    }
  } // End _S0W  
} // End PXSX
*/
//
// PCIe RTD3 - for slot#4
//
Scope (\_SB.PCI0.RP04)
{
  Name(_PR0, Package(){PXP4})
  Name(_PR3, Package(){PXP4})
  Name(WKEN, 0)

  Method(_S0W, 0)
  { // PMEs can be generated from D3cold
    If(And(\_SB.OSCO, 0x04))
    { // OS comprehends D3cold, as described via \_SB._OSC
      Return(4)
    } Else {
      Return(3)
    }
  } // End _S0W

  Method(_DSW, 3)
  {
    If(Arg1)
    { // Entering Sx, need to disable WAKE# from generating runtime PME
        Store(0, WKEN)
    } Else {  // Staying in S0
      If(LAnd(Arg0, Arg2)) // Exiting D0 and arming for wake
      { // Set PME
        Store(1, WKEN)
      } Else { // Disable runtime PME, either because staying in D0 or disabling wake 
        Store(0, WKEN)
      }
    }
  } // End _DSW


 // Define a power resource for PCIe RP4 D0-D3hot
  PowerResource(PXP4, 0, 0)
  {
    Name(OFFC, 0)     // _OFF Counter
    Name(ONC, 0)      // _ON Counter
    Name(PWRS, 0xFF)  // current power status

    Method(_STA, 0)
    {
      If(LAnd(NAnd(RT16, 1), And(RT17, 1))){
        Return (1) //
      } else {
        Return (0)
      }
    }
    Method(_ON) // Turn on core power to PCIe Slot4
    {
      Increment(ONC)
      //Power ON for Slot4
      And(\RT16, 0x0, \RT16) //Apply power
      Sleep(PEP0)     // Sleep for programmable delay
      Or(\RT17, 0x1, \RT17) //De-assert PERST#
      Store(1, PWRS)
    }

    Method(_OFF) // Turn off core power to PCIe Slot1
    {
      Increment(OFFC)
      //Power OFF for Slot4
      And(\RT17, 0x0, \RT17)
      Or(\RT16, 0x1, \RT16)
      Store(1, \_SB.PCI0.RP04.LDIS)
      Store(0, \_SB.PCI0.RP04.LDIS) //toggle link disable

      If(\_SB.PCI0.RP04.WKEN) {
        Store(0, PWRS)
          Store(1, \SS11) //Clear SMI status
          Store(1, \SE11) //Enable SMI
      }
    }
  } // End PXP4

} // End RP04
/*
Scope(\_SB.PCI0.RP04.PXSX)
{ // PCIe Device beneath RP04
  Method(_S0W, 0)
  { // PMEs can be generated from D3cold
    If(And(\_SB.OSCO, 0x04))
    { // OS comprehends D3cold, as described via \_SB._OSC
      Return(4)
    } Else {
      Return(3)
    }
  } // End _S0W  
} // End PXSX
*/

//
// PCIe RTD3 - for slot#5
//
Scope (\_SB.PCI0.RP05)
{
  Name(_PR0, Package(){PXP5})
  Name(_PR3, Package(){PXP5})
  Name(WKEN, 0)

  Method(_S0W, 0)
  { // PMEs can be generated from D3cold
    If(And(\_SB.OSCO, 0x04))
    { // OS comprehends D3cold, as described via \_SB._OSC
      Return(4)
    } Else {
      Return(3)
    }
  } // End _S0W

  Method(_DSW, 3)
  {
    If(Arg1)
    { // Entering Sx, need to disable WAKE# from generating runtime PME
        Store(0, WKEN)
    } Else {  // Staying in S0
      If(LAnd(Arg0, Arg2)) // Exiting D0 and arming for wake
      { // Set PME
        Store(1, WKEN)
      } Else { // Disable runtime PME, either because staying in D0 or disabling wake 
        Store(0, WKEN)
      }
    }
  } // End _DSW


  // Define a power resource for PCIe RP5 D0-D3hot
  PowerResource(PXP5, 0, 0)
  {
    Name(OFFC, 0)     // _OFF Counter
    Name(ONC, 0)      // _ON Counter
    Name(PWRS, 0xFF)  // current power status

    Method(_STA, 0)
    {
      If(LAnd(NAnd(RT32, 1), And(RT35, 1))){
        Return (1)
      } else {
        Return (0)
      }

    } 
    Method(_ON) // Turn on core power to PCIe Slot1
    {
     P8XH(0,0x11, 1)
     P8XH(1,0x50, 1)
      Increment(ONC)
      //Power ON for Slot5
      And(\RT32, 0x0, \RT32)
      Store(1, PWRS)
      Sleep(PEP0)     // Sleep for programmable delay
      Or(\RT35, 0x1, \RT35)
    }

    Method(_OFF) // Turn off core power to PCIe Slot5
    {
      Increment(OFFC)
      //Power OFF for Slot5
      And(\RT35, 0x0, \RT35)
      Or(\RT32, 0x1, \RT32)

      Store(1, \_SB.PCI0.RP05.LDIS)
      Store(0, \_SB.PCI0.RP05.LDIS) //toggle link disable
      Store(0, PWRS)
      Sleep(PEP3) // user selectable delay
      If(\_SB.PCI0.RP05.WKEN) {
          Store(1, \SS13) //Clear SMI status
          Store(1, \SE13) //Enable SMI
      }
    }
  } // End PXP5

} // End RP05
/* For Debug
Scope(\_SB.PCI0.RP05.PXSX)
{ // PCIe Device beneath RP05
  Method(_S0W, 0)
  { // PMEs can be generated from D3cold
    If(And(\_SB.OSCO, 0x04))
    { // OS comprehends D3cold, as described via \_SB._OSC
      Return(4)
    } Else {
      Return(3)
    }
  } // End _S0W  
} // End PXSX
*/

//
// GPE Event Handler
//

Scope(\_GPE) {
  //
  // GPIO19 routed to GPE0b[25] => 32+25 = 57(0x39) = PCIE slot 3 wake event
  //
  
  Method(_L39) {
      // PCIe WAKE#
      Increment(\_SB.PCI0.RP03.WC)
      If(\_SB.PCI0.RP03.WKEN){
        Store(0, \_SB.PCI0.LPCB.GR19) //Mask SCI - to stop SCI flooding
        Notify(\_SB.PCI0.RP03, 0x02)  // DeviceWake
      }
  }
  
  //
  // GPI03 = SMSC 1007 nIO_SMI routed to PCH GPIO 3
  //
  Method(_L13) {
      /*
      If(LAnd(\_SB.PCI0.RP04.WKEN, SS11)){
        Store(0, SE11)
        Store(1, SS11)
        Notify(\_SB.PCI0.RP04, 0x02)  // DeviceWake
      }
      If(LAnd(\_SB.PCI0.RP05.WKEN, SS13)){
        Store(0, SE13)
        Store(1, SS13)
        Notify(\_SB.PCI0.RP05, 0x02)  // DeviceWake
      }
      */
  }
  //
  // GPIO11 - ZPODD DA 
  //
  Method(_L1B){
    //
    // Do nothing if Desktop platform
    //
    If(LEqual(PFLV,FDTP)) {
      Return ()
    }

    //
    // Toggle GPI invert
    //
    If(LEqual(And(\GL01, 0x08),0x08)) { // GPI11 is high
      Or(\GIV1, 0x08, \GIV1) //GP_INV should be set to trigger low 
    }Else {
      And(\GIV1, 0xF7, \GIV1) //GP_INV should be set to trigger high
    } 

    // Only do this if ZPODD not under _DSM control
    If(LEqual(\_SB.PCI0.SAT0.UDSM, Zero)) {
      //
      // Notify OSPM
      //
       //If(WKEN) {  // _DSW called to enable ODD to generate a wake event
        //send notification for Port 2
        Notify(\_SB.PCI0.SAT0.PRT2, 2)  // Device Wake to Device (Windows)
       //}
    } Else {
    
      // Drive GPIO68 to High to power on device.
      //
      Or(\GL08, 0x10, \GL08)
      //
      // Notify the OSPM
      //
      Notify(\_SB.PCI0.SAT0, 0x82)
    } // \_SB.PCI0.SAT0.UDSM
    Return ()
  }
}

//
// AHCI RTD3
//
Scope(\_SB.PCI0.SAT0) {
  //
  // _DSM Device Specific Method supporting AHCI LPM/DEVSLP
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index
  // Arg3: Package Parameters
  //
  Name(DRV, 0)  // Storage for _DSM Arg3 parameter 0
  Name(PWR, 0)  // Storage for _DSM Arg3 parameter 1
  Name(UDSM, 0) // Use _DSM for ZPODD control. 0 = Disabled; 1 = Enabled

  Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
    If (LEqual(Arg0, ToUUID ("E4DB149B-FCFE-425b-A6D8-92357D78FC7F"))) {
      //
      // Switch by function index
      //
      Switch (Arg2) {
        Case (0) {
          // Standard query - A bitmask of functions supported
          // Supports function 0-3
          Name(RBUF, Buffer(1){0x00})
          CreateByteField (RBUF, 0, SFUN)
          Store (0x0F, SFUN)
          Return (RBUF)
        }
        Case (1) {
          // Query Device IDs (Addresses) of children where drive power and/or PHYSLP are supported
          //Return (Package(){0x0000FFFF, 0x0001FFFF, 0x0002FFFF}) //Debug - All Ports supports Link control
          Return (Package(){0x0000FFFF}) //Only Port0 in MintSpring supports Link control

        }
        //
        // Control power to device.
        //
        Case (2) {
          Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
          Store(ToInteger(DerefOf(Index(Arg3, One))), PWR)

          Switch(DRV){
            Case (0x0000FFFF){
              If(PWR){ // Applying Power
                // Turn on drive and link power to drive at Port 0
                Store(1, RT14) // De assert physlp -> Link ON, LED ON
                ^PRT0.P0PR._ON()
              } Else {  // Remove Link Power
                 Store(0, RT14) // Assert physlp -> Link OFF, LED OFF
              }
            }            
            //We don't have Link power control in Mintspring. so the code below can be removed.
            Case (0x0001FFFF){
              If(PWR){ // Applying Power
                // Turn on drive and link power to drive at Port 1
                \_SB.PCI0.SAT0.PRT1.P1PR._ON()
              } Else {  // Remove Link Power
                NoOp    // No link power control on these ports.
              }
            }            
            Case (0x0002FFFF){
              // Turn on drive and link power to drive at Port 2
              If(PWR){ // Applying Power
                // Turn on drive and link power to drive at Port 2
                //^PRT2.P2PR._ON()
              } Else {  // Remove Link Power
                NoOp    // No link power control on these ports
              }
            }            
            Case (Ones){
              // Turn on drive and link power to all drives
                If(PWR){ // Applying Power
                   Store(1, RT14) // De assert physlp -> Link ON, LED ON (Link might be disabled)
                  ^PRT0.P0PR._ON()
                  \_SB.PCI0.SAT0.PRT1.P1PR._ON()
                  ^PRT2.P2PR._ON()
                }
            }            
          }
          Return (0)
        }
        //
        // Current status of Device/Link of Port
        //
        Case (3){
          Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
          Switch(DRV){
            Case (0x0000FFFF){
              Store(ShiftRight(NAnd(GL04, 0x04), 2), Local0)      // Bit0 => Device power state
              If(LEqual(RT14,1)){     //If Link ON
                Or(Local0, 2, Local0) //Bit1 => Link power state
              }
              Return (Local0)
            }

            Case (0x0001FFFF){
              Store(ShiftRight(NAnd(GL04, 0x08), 2), Local0)      // Bit0 => Device power state
              Or(Local0, 2, Local0) //Bit1 => Link power state
              Return (Local0)
            }

            Case (0x0002FFFF){
              Store(ShiftRight(And(GL08, 0x010), 2), Local0)      // Bit0 => Device power state
              Or(Local0, 2, Local0) //Bit1 => Link power state
              Return (Local0)
            }

        Default {
              Return (Ones)
            }
        }
      }
       Default {
         Return (0)
       }
      } //Switch (Arg2) 
    } Else {
      Return (0)
    }
  }

  // D0 Method for SATA HBA(for debug)
  Method(_PS0,0,Serialized)
  {
  }

  // D3 Method for SATA HBA(for debug)
  Method(_PS3,0,Serialized)
  {
  }

  Scope(PRT0) {
    Name(_PR0, Package(){P0PR})
    Name(_PR3, Package(){P0PR})
  

  PowerResource(P0PR, 0, 0){ // SATA_PWR_EN#0 is PCH GPIO34
    Method(_STA){
      If(And(GL04, 0x04)) {
        Return(0x00)
      } Else {
        Return(0x01)
      }
    }
    
    Method(_ON, 0)
    { // Turn on the PFET
      And(GL04, 0xfb, GL04)
      // Delay for power ramp
      Sleep(16)
    }

    Method(_OFF, 0)
    { // Turn off the PFET
      // TODO: when removing power, need to ensure DEVSLP is either deasserted or reverts to an input
      //(safer, no need to keep track if it was asserted before)
      Store(1, RT14) // De assert physlp -> Link ON, LED ON
      Or(GL04, 0x04, GL04)
    }
  } // End P0PR
  } // End PRT0

  Scope(PRT1) {

    Name(_PR0, Package(){P1PR})
    Name(_PR3, Package(){P1PR})
  

  PowerResource(P1PR, 0, 0){ // SATA_PWR_EN#0 is PCH GPIO35 on Emerald Lake
    Method(_STA){
//      Or(GU04, 8, GU04) // GPIO init code incorrect- make this GPIO non-native
      If(And(GL04, 0x08)) {
        Return(0x00)
      } Else {
        Return(0x01)
      }
    }
    
    Method(_ON, 0)
    { // Turn on the PFET
      And(GL04, 0xf7, GL04)
      // Delay for power ramp
      Sleep(16)
    }

    Method(_OFF, 0)
    { // Turn off the PFET
      Or(GL04, 8, GL04)
    }
  } // End P1PR
  } //End PRT1

  Scope(PRT2) {  // Port 2- Cable Connect power
          //
          // _DSM Device Specific Method supporting SATA ZPODD function
          //
          // Arg0: UUID Unique function identifier
          // Arg1: Integer Revision Level
          // Arg2: Integer Function Index
          // Arg3: Package Parameters
          //
          Method (_DSM, 4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
            If (LEqual(Arg0, ToUUID ("bdfaef30-aebb-11de-8a39-0800200c9a66"))) {
              //
              // Switch by function index
              //
              Switch (ToInteger(Arg2)) {
                //
                // Standard query - A bitmask of functions supported
                // Supports function 0-5
                //
                Case (0) {
                  switch(ToInteger(Arg1)) {
                    case(1) {
                        //
                        // Return - 0 (No Functions supported) for Desktop Platforms
                        //
                         If(LEqual(PFLV,FDTP)) { 
                            Return (Buffer () {0})
                         }
                         Return (Buffer () {0xF})
                    }
                    Default {Return (Buffer () {0})}
                  }
                }
                //
                // Enable ZPODD feature
                //
                Case (1) {
                  //
                  // Enable Power ON/OFF
                  //
                  // Function 1: Enable Power OFF/ON. Enables the Power Off/ON hardware in the system.
                 // When called, the BIOS is informed that the host software is using this interface. 
                 If(LEqual(\_SB.PCI0.SAT0.UDSM, Zero)){ //if Ownership not acquired
                     Store (1, \_SB.PCI0.SAT0.UDSM)    // Indicate ZPODD _DSM control enabled
                 }
                  Store(2, \_SB.PCI0.LPCB.GR0B) // Enable SCI (for ZPODD wake) in LPC register
                  Return (1)
                }
                //
                // Power Off Device
                //
                Case (2) {
                  //
                  // Drive GPIO68 to low to power off device.
                  //
                  And(\GL08, 0xEF, \GL08)
                  Return (1)
               }
              //
              // Power ON Device
              //
              Case (3) {
                  Or(\GL08, 0x10, \GL08)
                  Sleep(16) // To turn the FET
                  Return (1)
               }
              Default {
                Return (0)
              }
            }
          } Else {
            Return (0)
          }
        }
    Name(_PR0, Package(){P2PR})
    Name(_PR3, Package(){P2PR})

    Name(WKEN, 0) // Device on this port wake enabled?

    Method(_S0W, 0)
    { // PMEs can be generated from D3cold
      Store(2, \_SB.PCI0.LPCB.GR0B) // Enable SCI (for ZPODD wake) in LPC register
      If(And(\_SB.OSCO, 0x04))
      { // OS comprehends D3cold, as described via \_SB._OSC
        Return(4)
      } Else {
        Return(3)
      }
    } // End _S0W

    Method(_DSW, 3)
    {
      If(Arg1)
      { // Entering Sx, need to disable DA from generating runtime PME
        Store(0, WKEN)
      } Else {  // Staying in S0
        If(LAnd(Arg0, Arg2)) // Exiting D0 and arming for wake
        { // Set WKEN to allow _OFF to enable the wake event
          Store(1, WKEN)
        } Else { // Clear WKEN to allow _OFF to enable the wake event, 
                 // either because staying in D0 or disabling wake 
          Store(0, WKEN)
        }
      }
    } // End _DSW
    
  Method(_PRW, 0) { Return(GPRW(0x13, 4)) } // can wakeup from S4 state

    PowerResource(P2PR, 0, 0){
    Name(_STA, 0x01)
    
    Method(_ON, 0)
    {
      Store(0x01, _STA)    
      Or(GL08, 0x10, GL08)
      Sleep(16) // To turn the FET           
    }

    Method(_OFF, 0)
    {
      //
      // Drive GPIO68 to low to power off device.
      //
      And(GL08, 0xEF, GL08)
      Store(0, _STA)
    }
  } // End P2PR
  } //End PRT2

  PowerResource(V0PR, 0, 0)
  { // Power Resource Aggregate for RAID volume 0
    Name(_STA, 0x01)
    Name(PMAP, 0)
  
    Method(_ON, 0)
    { // Turn on the PFET
      If(Not(_STA)){
        Store(0x01, _STA)
        If(And(PMAP, 0x01))
        { // Turn On Port 0 Power
          And(GL04, 0xfb, GL04)
        }
        If(And(PMAP, 0x02))
        { // Turn On Port 1 Power
          And(GL04, 0xf7, GL04)
        }
        If(And(PMAP, 0x04))
        { // Turn On Port 2 Power
          Or(GL08, 0x10, GL08)
        }
        // Delay for power ramp
        Sleep(16)
      }
    }

    Method(_OFF, 0)
    { // Turn off the PFET
      If(_STA){
        Store(0x01, _STA)
        If(And(PMAP, 1))
        { // Turn Off Port 0 Power
          Or(GL04, 0x04, GL04)
        }
        If(And(PMAP, 2))
        { // Turn Off Port 1 Power
          Or(GL04, 0x08, GL04)
        }
        If(And(PMAP, 4))
        { // Turn Off Port 2 Power
          And(GL08, 0xEF, GL08)
        }
      }
    }
  } // End V0PR

  PowerResource(V1PR, 0, 0)
  { // Power Resource Aggregate for RAID volume 1
    Name(_STA, 0x01)
    Name(PMAP, 0)
  
    Method(_ON, 0)
    { // Turn on the PFET
      If(Not(_STA)){
        Store(0x01, _STA)
        If(And(PMAP, 0x01))
        { // Turn On Port 0 Power
          And(GL04, 0xfb, GL04)
        }
        If(And(PMAP, 0x02))
        { // Turn On Port 1 Power
          And(GL04, 0xf7, GL04)
        }
        If(And(PMAP, 0x04))
        { // Turn On Port 2 Power
          Or(GL08, 0x10, GL08)
        }
        // Delay for power ramp
        Sleep(16)
      }
    }

    Method(_OFF, 0)
    { // Turn off the PFET
      If(_STA){
        Store(0x01, _STA)
        If(And(PMAP, 1))
        { // Turn Off Port 0 Power
          Or(GL04, 0x04, GL04)
        }
        If(And(PMAP, 2))
        { // Turn Off Port 1 Power
          Or(GL04, 0x08, GL04)
        }
        If(And(PMAP, 4))
        { // Turn Off Port 2 Power
          And(GL08, 0xEF, GL08)
        }
      }
    }
  } // End V1PR

  PowerResource(V2PR, 0, 0)
  { // Power Resource Aggregate for RAID volume 2
    Name(_STA, 0x01)
    Name(PMAP, 0)
  
    Method(_ON, 0)
    { // Turn on the PFET
      If(Not(_STA)){
        Store(0x01, _STA)
        If(And(PMAP, 0x01))
        { // Turn On Port 0 Power
          And(GL04, 0xfb, GL04)
        }
        If(And(PMAP, 0x02))
        { // Turn On Port 1 Power
          And(GL04, 0xf7, GL04)
        }
        If(And(PMAP, 0x04))
        { // Turn On Port 2 Power
          Or(GL08, 0x10, GL08)
        }
        // Delay for power ramp
        Sleep(16)
      }
    }

    Method(_OFF, 0)
    { // Turn off the PFET
      If(_STA){
        Store(0x01, _STA)
        If(And(PMAP, 1))
        { // Turn Off Port 0 Power
          Or(GL04, 0x04, GL04)
        }
        If(And(PMAP, 2))
        { // Turn Off Port 1 Power
          Or(GL04, 0x08, GL04)
        }
        If(And(PMAP, 4))
        { // Turn Off Port 2 Power
          And(GL08, 0xEF, GL08)
        }
      }
    }
  } // End V2PR

  Device(VOL0) {
    Name(_ADR,0x0080FFFF)  // RAID Volume 0

    Name(_PR0, Package(){V0PR})
    Name(_PR3, Package(){V0PR})

    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (LEqual(Arg0, ToUUID ("E03E3431-E510-4fa2-ABC0-2D7E901245FE"))) {
        //
        // Switch by function index
        //
        Switch (Arg2) {

          Case (0) {
            // Standard query - A bitmask of functions supported
            // Supports function 0-3
            Name(RBUF, Buffer(1){0x00})
            CreateByteField (RBUF, 0, SFUN)
            Store (0x03, SFUN)
            Return (RBUF)
          }

          Case (1) {// Assign ports to this RAID volume
            Store(DeRefOf(Index(Arg3,0)), ^^V0PR.PMAP) // Assign port mapping to the volume's power resource
            Return(Package(0){})
          }
          Default {
           Return (0)
          }
        } //Switch (Arg2) 
      } Else {
        Return (0)
      }
    }
  }

  Device(VOL1) {
    Name(_ADR,0x0081FFFF)  // RAID Volume 1

    Name(_PR0, Package(){V1PR})
    Name(_PR3, Package(){V1PR})

    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (LEqual(Arg0, ToUUID ("E03E3431-E510-4fa2-ABC0-2D7E901245FE"))) {
        //
        // Switch by function index
        //
        Switch (Arg2) {

          Case (0) {
            // Standard query - A bitmask of functions supported
            // Supports function 0-3
            Name(RBUF, Buffer(1){0x00})
            CreateByteField (RBUF, 0, SFUN)
            Store (0x03, SFUN)
            Return (RBUF)
          }

          Case (1) {// Assign ports to this RAID volume
            Store(DeRefOf(Index(Arg3,0)), ^^V1PR.PMAP) // Assign port mapping to the volume's power resource
            Return(Package(0){})          
          }
          Default {
           Return (0)
          }
        } //Switch (Arg2) 
      } Else {
        Return (0)
      }
    }
  }

  Device(VOL2) {
    Name(_ADR,0x0082FFFF)  // RAID Volume 2

    Name(_PR0, Package(){V2PR})
    Name(_PR3, Package(){V2PR})

    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (LEqual(Arg0, ToUUID ("E03E3431-E510-4fa2-ABC0-2D7E901245FE"))) {
        //
        // Switch by function index
        //
        Switch (Arg2) {

          Case (0) {
            // Standard query - A bitmask of functions supported
            // Supports function 0-3
            Name(RBUF, Buffer(1){0x00})
            CreateByteField (RBUF, 0, SFUN)
            Store (0x03, SFUN)
            Return (RBUF)
          }

          Case (1) {// Assign ports to this RAID volume
            Store(DeRefOf(Index(Arg3,0)), ^^V2PR.PMAP) // Assign port mapping to the volume's power resource
            Return(Package(0){})          
          }
          Default {
           Return (0)
          }
        } //Switch (Arg2) 
      } Else {
        Return (0)
      }
    }
  }
}

  Scope(\_SB.PCI0.EHC2){
    Method(_S0W, 0)
    { // PMEs can be generated from D3(hot)
        Return(3)
    } // End _S0W

    // D0 Method for EHCI2 Host Controller
    Method(_PS0,0,Serialized)
    {
    }

    // D3 Method for EHCI2 Host Controller
    Method(_PS3,0,Serialized)
    {
      Store(1, \_SB.PCI0.EHC2.PMES) //Clear PME status
      Store(1, \_SB.PCI0.EHC2.PMEE) //Enable PME
    }
  }

  Scope(\_SB.PCI0.XHC){
    Method(_S0W, 0)
    { // PMEs can be generated from D3(hot)
        Return(3)
    } // End _S0W

    // D0 Method for xHCI Host Controller
    Method(_PS0,0,Serialized)
    {
    }

    // D3 Method for xHCI Host Controller
    Method(_PS3,0,Serialized)
    {
      Store(1, \_SB.PCI0.XHC.PMES) //Clear PME status
      Store(1, \_SB.PCI0.XHC.PMEE) //Enable PME
    }
  }

  Scope(\_SB.PCI0.XHC.RHUB.HS01){
    Method(_PR0, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF),0x01))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return(Package() {})
      }
    } // End of Method _PR0
    Method(_PR2, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF),0x01))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return(Package() {})
      }
    } // End of Method _PR2
    Method(_PR3, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF),0x01))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return (Package() {})
      }
    } // End of Method _PR3
  }

  Scope(\_SB.PCI0.XHC.RHUB.HS02){
    Method(_PR0, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF0),0x10))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return(Package() {})
      }
    } // End of Method _PR0
    Method(_PR2, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF0),0x10))
      {
              Return(Package(){\PX01})
      }
      Else
      {
        Return(Package() {})
      }
    } // End of Method _PR2
    Method(_PR3, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF0),0x10))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return (Package() {})
      }
    } // End of Method _PR3

  }

  Scope(\_SB.PCI0.XHC.RHUB.SSP1){
    Method(_PR0, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF),0x02))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return(Package() {})
      }
    }
    Method(_PR2, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF),0x02))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return(Package() {})
      }
    }
    Method(_PR3, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF),0x02))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return (Package() {})
      }
    }// End of method _PR3
  } //  End of Scope(\_SB.PCI0.XHC.RHUB.SSP1)

  Scope(\_SB.PCI0.XHC.RHUB.SSP2){

    Method(_PR0, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF0),0x20))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return(Package() {})
      }
    }
    Method(_PR2, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF0),0x20))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return(Package() {})
      }
    }
    Method(_PR3, Zero, NotSerialized, 0, PkgObj)
    {
      If(LEqual(And(XHPR,0xF0),0x20))
      {
        Return(Package(){\PX01})
      }
      Else
      {
        Return (Package() {})
      }
    }// End of method _PR3
  }//Scope(\_SB.PCI0.XHC.RHUB.SSP2)

  Scope(\_SB.PCI0.EHC2.HUBN.PR01.PR11){
    Name(_PR0, Package(){\PX89})
    Name(_PR2, Package(){\PX89})
    Name(_PR3, Package(){\PX89})

  }

  Scope(\_SB.PCI0.EHC2.HUBN.PR01.PR12){
    Name(_PR0, Package(){\PX89})
    Name(_PR2, Package(){\PX89})
    Name(_PR3, Package(){\PX89})

  }

  Scope(\) {
  PowerResource(PX01, 0, 0){ //Common power rail for USB2.0 ports 0,1 /USB3.0 ports 1,2  - GPIO 40
    Method(_STA){
      If(And(GL05, 0x02)) { //GPIO41 => GL05 BIT0
        Return(0x01)
      } Else {
        Return(0x00)
      }
    }
    
    Method(_ON, 0)
    { // Turn on
      Or(GL05, 2, GL05)
      // Delay for power ramp
      Sleep(16)
    }

    Method(_OFF, 0)
    { // Turn off
      And(GL05, 0xfd, GL05)
    }
  } // End PX01

  PowerResource(PX89, 0, 0){ //Common power rail for USB2.0 ports 8, 9 (stacked with LAN conn; EHCI) - GPIO 14
    Method(_STA){
      If(And(GL01, 0x40)) { //GPI14 => GL01 BIT6
        Return(0x01)
      } Else {
        Return(0x00)
      }
    }
    
    Method(_ON, 0)
    { // Turn on
      Or(GL01, 0x40, GL01)
      // Delay for power ramp
      Sleep(16)
    }

    Method(_OFF, 0)
    { // Turn off
      And(GL01, 0x0bf, GL01)
    }
  } // End PX89

} //Scope(\) 

} //If(LAnd(LEqual(BID,BBR),LEqual(RTD3,1)))

} // End SSDT