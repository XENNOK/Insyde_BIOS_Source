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
    "HSW-FFRD",
    0x1000
    )
{
#define FFRDRTD3 1
Include ("RTD3Common.asl")
#undef FFRDRTD3

External(PEBS, IntObj)        // PCI Express BAR address
External(BHB, IntObj)         // Harris Beach FFRD Board
External(\_SB.PCI0.XHC.RHUB.HS05, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.HS06, DeviceObj)
External(\_SB.PCI0.SDHC, DeviceObj) //SDIO
External(\_SB.PCI0.SDHC.WI01, DeviceObj) //SDIO Wifi
External(\_SB.PCI0.I2C1.TPD7, DeviceObj) // Precision Elantech touchpad
External(\_SB.PCI0.I2C1.TPD8, DeviceObj) // Precision Synaptics touchpad

If(LAnd(LEqual(BID,BHB),LEqual(RTD3,1))) {
//
// PCIe RTD3 - for slot#1
//
Scope(\_SB.PCI0.RP01)
{
  Name(SLOT, 1)
  Name(RSTG, 77) // reset GPIO NGFF_WIFI_RST_N
  Name(PWRG, 83) // power GPIO
  Name(RONP, 1)  // reset on polarity (DE-ASSERT)
  Name(PONP, 1)  // power on polarity
  Name(NGFF, 1)  // Is Slot NGFF form factor 1- Yes 0- No
  Name(SCLK, 0x1)  // Source Clock Enable Bit Location(BIT0)
  Include("RTD3Pcie.asl")
}

//SATA - START
  Scope(\_SB.PCI0.SAT0) {

    // D0 Method for SATA HBA
    Method(_PS0,0,Serialized)
    {
    }

    // D3 Method for SATA HBA
    Method(_PS3,0,Serialized)
    {
    }

    OperationRegion(SMIO,PCI_Config,0x24,4) 
    Field(SMIO,AnyAcc, NoLock, Preserve) {
        Offset(0x00), // SATA MABR6
        MBR6, 32,       // SATA ABAR
    }

    OperationRegion(ST93,PCI_Config,0x93,1) 
    Field(ST93,AnyAcc, NoLock, Preserve) {
        Offset(0x00),
        P0P, 1,       // Port 0 Present
        P1P, 1,       // Port 1 Present
        P2P, 1,       // Port 2 Present
        P3P, 1,       // Port 3 Present
    }

    Scope(PRT1) {
      // Store boot drive connection status
      // Initialize to drive connected, value updated in _INI
      Name(CONN, 1)

      Method (_INI, 0)
      {
        // If no drive connected on boot, update connection status and power gate the port
        If(LEqual(P1P, 0))
        {
          Store (0, CONN)
          \_SB.WTGP (90, 1)
        }
      }

      PowerResource(P1PR, 0, 0){
      Name(OFTM, Zero) // Time returned by Timer() when the Power resource was turned OFF

        Method(_STA){
          If(LEqual(\_SB.RDGP(90),1)) { //GPIO90
            Return(0x00)
          } Else {
            Return(0x01)
          }
        }
        Method(_ON, 0)
        {
          if(And(RCG0,0x10)) // RTD3Config0/RCG0 BIT 4 - Setup option to powergate Port 1
          {
            // If there was a device present on boot, power on device
            If(LEqual(CONN, 1))
            {
              If(LNotEqual(^OFTM, Zero)) { // if OFTM != 0 => Disk as turned OFF by asl
                Divide(Subtract(Timer(), ^OFTM), 10000, , Local0) //Store Elapsed time in ms
                Store(Zero, ^OFTM) // Reset OFTM to zero to indicate minimum 50ms requirement does not apply when _ON called next time
                If(LLess(Local0, 50)) // Do not sleep if already past the delay requirement
                {
                  Sleep(Subtract(50, Local0)) // Sleep 100ms - time elapsed
                }
              }

              SGON(90, Zero) // VR stagger GPIO90 to low to power on device
            }
          } // if(And(RCG0,0x10))
        } // End of ON Method
        Method(_OFF, 0)
        {
         if(And(RCG0,0x10)) // RTD3Config0/RCG0 BIT 4 - Setup option to powergate Port 1
         {
           Add(\_SB.PCI0.SAT0.MBR6,0x198 ,Local0)
           OperationRegion(PSTS, SystemMemory, Local0, 0x18) 
           Field(PSTS, DWordAcc, NoLock, Preserve) {
             Offset(0x0),
             CMST, 1,  //PxCMD.ST
             CSUD, 1,  //PxCMD.SUD
             , 2,
             CFRE, 1,  //PxCMD.FRE
             Offset(0x10),
             SDET, 4,  //PxSSTS.DET
             Offset(0x14),
             CDET, 4   //PxSCTL.DET
            }
            If(LOr(LEqual(SDET, 1), LEqual(SDET, 3))){  //Offline flow only if Device detected and Phy not offline
              //Clear ST (PxCMD.ST) 198[0]
              Store(0, CMST)
              //Clear FRE 198[4]
              Store(0, CFRE)
              //Clear SUD (PxCMD.SUD) 198[1]
              Store(0, CSUD)
              //Set DET to 4 (PxSCTL.DET) 1ac[3:0]
              Store(4, CDET)
              Sleep(16)
              //Wait until PxSSTS.DET == 4
              While(LNotEqual(SDET, 4)){
                Sleep(16)
              }
            }
            // Drive GPIO90 to High to power off device
            \_SB.WTGP(90,1)
            Store(Timer(), ^OFTM) // Store time when Disk turned OFF(non-zero OFTM indicate minimum 50ms requirement does apply when _ON called next time)
          } // if(And(RCG0,0x10))
        } // End of OFF method
      } // End P1PR

      Name(_PR0, Package(){P1PR})
      Name(_PR3, Package(){P1PR})

    } //End PRT1
    
    // PRT2 . Controlled by EC pin (PF6/PWMU4A/ExCTS).
    Scope(PRT2) {
      // Store boot drive connection status
      // Initialize to drive connected, value updated in _INI
      Name(CONN, 1)

      Method (_INI, 0)
      {
        // If no drive connected on boot, update connection status and power gate the port
        If(LEqual(P2P, 0))
        {
          Store (0, CONN)
          \_SB.WTGP (86, 1)
        }
      }

      PowerResource(P2PR, 0, 0){
        Name(OFTM, Zero) // Time returned by Timer() when the Power resource was turned OFF

        Method(_STA){
          If(LEqual(\_SB.RDGP(86),1)) { // GPIO86
            Return(0x00)
          } Else {
            Return(0x01)
          }
          
        }
        Method(_ON, 0)
        {
          if(And(RCG0,0x20)) // RTD3Config0/RCG0 BIT 5 - Setup option to powergate Port 2
          {
            // If there was a device present on boot, power on device
            If(LEqual(CONN, 1))
            {
              If(LNotEqual(^OFTM, Zero)) { // if OFTM != 0 => Disk as turned OFF by asl
                Divide(Subtract(Timer(), ^OFTM), 10000, , Local0) // Store Elapsed time in ms
                Store(Zero, ^OFTM) // Reset OFTM to zero to indicate minimum 50ms requirement does not apply when _ON called next time
                If(LLess(Local0, 50)) // Do not sleep if already past the delay requirement
                {
                  Sleep(Subtract(50, Local0)) //Sleep 100ms - time elapsed
                }
              }

              SGON(86, Zero) // VR stagger GPIO86 to low to power on device
            }
          } // if(And(RCG0,0x20))
        } // End of ON Method
        Method(_OFF, 0)
        {
          if(And(RCG0,0x20)) // RTD3Config0/RCG0 BIT 5 - Setup option to powergate Port 2
          {
            Add(\_SB.PCI0.SAT0.MBR6,0x218 ,Local0)
            OperationRegion(PSTS, SystemMemory, Local0, 0x18) 
            Field(PSTS, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              CMST, 1,  //PxCMD.ST
              CSUD, 1,  //PxCMD.SUD
              , 2,
              CFRE, 1,  //PxCMD.FRE
              Offset(0x10),
              SDET, 4,
              Offset(0x14),
              CDET, 4   //PxSCTL.DET
              }
            If(LOr(LEqual(SDET, 1), LEqual(SDET, 3))){  //Offline flow only if Device detected and Phy not offline
              //Clear ST (PxCMD.ST) 
              Store(0, CMST)
              //Clear FRE 
              Store(0, CFRE)
              //Clear SUD (PxCMD.SUD)
              Store(0, CSUD)
              //Set DET to 4 (PxSCTL.DET)
              Store(4, CDET)
              Sleep(16)
              //Wait until PxSSTS.DET == 4
              While(LNotEqual(SDET, 4)){
                Sleep(16)
              }
            }
            // Drive GPIO86 to High to power off device
            \_SB.WTGP(86,1)
            Store(Timer(), ^OFTM) // Store time when Disk turned OFF(non-zero OFTM indicate minimum 50ms requirement does apply when _ON called next time)
          } // if(And(RCG0,0x20))
        } // End of OFF method
      } // End P2PR

      Name(_PR0, Package(){P2PR})
      Name(_PR3, Package(){P2PR})

    } //End PRT2


    //
    // _DSM Device Specific Method supporting AHCI DEVSLP
    //
    // Arg0: UUID Unique function identifier
    // Arg1: Integer Revision Level
    // Arg2: Integer Function Index
    // Arg3: Package Parameters
    //
    Name(DRV, 0)  // Storage for _DSM Arg3 parameter 0
    Name(PWR, 0)  // Storage for _DSM Arg3 parameter 1

    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (LEqual(Arg0, ToUUID ("E4DB149B-FCFE-425b-A6D8-92357D78FC7F"))) {
        //
        // Switch by function index
        //
        Switch (Arg2) {
          Case (0) {
            // Standard query - A bitmask of functions supported
            // Supports function 0-3
            Return(0x0f)
          }
          Case (1) {
            // Query Device IDs (Addresses) of children where drive power and/or DevSleep are supported.
            // LPT-LP SATA HBA provides autonomous link (DevSleep) support, return a package of 0 elements
            Return( Package(){})		   // SATA HBA provides native DevSleep 
          }
          //
          // Control power to device.
          //
          Case (2) {
            Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
            Store(ToInteger(DerefOf(Index(Arg3, One))), PWR)

            Switch(DRV){
              Case (Ones){
                // Turn on drive and link power to all drives. Note that even though 
                If(PWR){ // Applying Power
                  //^PRT0.P0PR._ON()        // apply power to port 0 
                  \_SB.PCI0.SAT0.PRT1.P1PR._ON()        // apply power to port 1 
                  \_SB.PCI0.SAT0.PRT2.P2PR._ON()        // apply power to port 2 
                }
              }
            } //Switch(DRV)
            Return (0)
          } //Case (2)
          //
          // Current status of Device/Link of Port
          //
          Case (3){
            Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
            Switch(DRV){
              Case (0x0000FFFF){	// SATA Port 0
                // Bit0 => Device power state
                Store(1, Local0)      // always ON
                Return (Local0)
              }
              Case (0x0001FFFF){	// SATA Port 1
                // Bit0 => Device power state
                If(LEqual(\_SB.PCI0.SAT0.PRT1.P1PR._STA(), 0)){
                  Store(0, Local0)
                }Else{
                  Store(1, Local0)
                }
                Return (Local0)
              }
              Case (0x0002FFFF){	// SATA Port 2
                // Bit0 => Device power state
                If(LEqual(\_SB.PCI0.SAT0.PRT2.P2PR._STA(), 0)){
                  Store(0, Local0)
                }Else{
                  Store(1, Local0)
                }
                Return (Local0)
              }
              Default {		// Invalid SATA Port - error
                Return (Ones)
              }
            }
          }
          Default {
            Return (0)
          }
        }
      } Else { // UUID does not match
        Return (0)
      }
    } //Method(_DSM)

  } //Scope(\_SB.PCI0.SAT0) 

// SATA - END

//USB - START
  Scope(\_SB.PCI0.XHC){
    Name(_S0W, 3)
  }

  Scope(\_SB.PCI0.XHC.RHUB){ //USB XHCI RHUB
    Method(_PS0,0,Serialized)
    {
      Store(0x00,USPP)
    }
    Method(_PS2,0,Serialized)
    {
    
      OperationRegion (XHCM, SystemMemory, And(\_SB.PCI0.XHC.MEMB,0xFFFF0000), 0x600)
      Field (XHCM, DWordAcc, NoLock, Preserve)
      {
        Offset(0x2),
        XHCV,16,
        Offset(0x480),
        HP01, 1, // HS port 1
        Offset(0x490),
        HP02, 1, // HS port 2
        Offset(0x4F0),
        HP08, 1, // HS port 8
        Offset(0x510),
        SP00, 1, // SS port 0
        Offset(0x520),
        SP01, 1, // SS port 1
        Offset(0x540),
        SP03 , 1, // SS port 3
      }
        If(LEqual(XHCV,0xFFFF)) // Controller in D3Hot(MEM_BASE not decoded)
        {
         Return()
        }      
        If(LAnd(LEqual(HP08,0x00),LEqual(SP03,0x00))) // SD Card reader
        {
          Or(USPP,0x01, USPP)
        }
        If(LAnd(LEqual(HP01,0x00),LEqual(SP00,0x00))) // USB(HS1 : 0x480 /SS0 : 0x510)
        {
          Or(USPP,0x02, USPP)
        }
        If(LAnd(LEqual(HP02,0x00),LEqual(SP01,0x00))) // USB(HS2 : 0x490 / SS1 : 0x520)
        {
          Or(USPP,0x04, USPP)
        }
    } // End of PS2 method
    Method(_PS3,0,Serialized)
    {
    } // End of _PS3 method
  }

Scope(\_SB.PCI0.XHC.RHUB.HS06){

    PowerResource(PX06, 0, 0){ // power rail for USB3.0 ports 6 - GPIO 84
      Method(_STA){
        If(LEqual(\_SB.RDGP(84),1)) {   //GPIO84
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(_ON, 0)
      { // Turn on
        SGON(84, One) // VR stagger GPIO84 to high to power on device
      }

      Method(_OFF, 0)
      { // Turn off
        \_SB.WTGP(84,0)
      }
    } // End PX06
  Name(_S0W,2) // Indicate WWAN can wake from D2 while in S0 using selective suspend
  Name(_PR0,Package(){\_SB.PCI0.XHC.RHUB.HS06.PX06}) // Power Resource required to support D0
  Name(_PR2,Package(){\_SB.PCI0.XHC.RHUB.HS06.PX06}) // Power Resource required to support D2
  Name(_PR3,Package(){\_SB.PCI0.XHC.RHUB.HS06.PX06}) // Power Resource required to support D3
}

  Scope(\_SB.PCI0.XHC.RHUB.HS05){
    PowerResource(PX05, 0, 0){ // power rail for USB2.0 ports 5 - GPIO 25
      Method(_STA){
        If(LEqual(\_SB.RDGP(25),1)) {   //GPIO25
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(_ON, 0)
      { // Turn on
        SGON(25, One) // VR stagger GPIO25 to high to power on device
      }

      Method(_OFF, 0)
      { // Turn off
        \_SB.WTGP(25,0)
      }
    } // End PX05  
  Name(_PR0,Package(){\_SB.PCI0.XHC.RHUB.HS05.PX05}) // Power Resource required to support D0
  Name(_PR2,Package(){\_SB.PCI0.XHC.RHUB.HS05.PX05}) // Power Resource required to support D2
  Name(_PR3,Package(){\_SB.PCI0.XHC.RHUB.HS05.PX05}) // Power Resource required to support D3
  }
  Scope(\_SB.PCI0.XHC){ //USB XHCI 
  
  Name(UPWR,0)
  Name(USPP,0)
  // Bit 0 : USB SD Card Reader
  // Bit 1 : USB(HS1 : 0x480 /SS0 : 0x510)
  // BIT 2 : USB(HS2 : 0x490 / SS1 : 0x520)

  
  Method(DUAM,0,Serialized)
  {

    OperationRegion (XHCM, SystemMemory, And(\_SB.PCI0.XHC.MEMB,0xFFFF0000), 0x600)
    Field (XHCM, DWordAcc, NoLock, Preserve)
    {
      Offset(0x2),
      XHCV,16,
      Offset(0x480),
      HP01, 1, // HS port 1
      Offset(0x490),
      HP02, 1, // HS port 2
      Offset(0x4F0),
      HP08, 1, // HS port 8
      Offset(0x510),
      SP00, 1, // SS port 0
      Offset(0x520),
      SP01, 1, // SS port 1
      Offset(0x540),
      SP03 , 1, // SS port 3
    }

  if(LNotEqual(\UAMS,0x00)) // User not present
  {
    Store(0x00,UPWR)
    // Enterng CS, Remove power if SD card not present

    If(LEqual(XHCV,0xFFFF)) // Controller in D3Hot(MEM_BASE not decoded)
    {
      If(And(USPP,0x01)) // SD Card reader
      {
        \_SB.WTGP(60,0)
        Or(UPWR,0x01, UPWR)
      }
      If(And(USPP,0x06))  // USB(HS1 : 0x480 /SS0 : 0x510)  & USB(HS2 : 0x490 / SS1 : 0x520)
      {
        \_SB.GP2N(40,1)         // Mask OC#(Switch to GPIO mode)
        \_SB.WTGP(13,0)         // Power gate GPIO13/43 at same time
        \_SB.WTGP(43,0)
        Or(UPWR,0x02, UPWR)
        Or(UPWR,0x04, UPWR)
      }
    }Else{ //    If(Lequal(XHCV,0xFFFF))
        If(LAnd(LEqual(HP08,0x00),LEqual(SP03,0x00))) // SD Card reader
        {
          \_SB.WTGP(60,0)
          Or(UPWR,0x01, UPWR)
        }
        If(LAnd(LAnd(LEqual(HP01,0x00),LEqual(SP00,0x00)), LAnd(LEqual(HP02,0x00),LEqual(SP01,0x00))))  // USB(HS1 : 0x480 /SS0 : 0x510) &USB(HS2 : 0x490 / SS1 : 0x520)
        {
          \_SB.GP2N(40,1)               // Mask OC#(switch to GPIO mode)
          \_SB.WTGP(13,0)               // Power gate GPIO13/43 at same time
          \_SB.WTGP(43,0)
          Or(UPWR,0x02, UPWR)
          Or(UPWR,0x04, UPWR)
        }
    }//    If(Lequal(XHCV,0xFFFF))
  }Else{ //if(LNotEqual(\UAMS,0x00))

    // Exiting CS , Apply power if power removed
    if(And(UPWR,0x01)) // SD Card reader
    {
      \_SB.WTGP(60,1)
    }
    if(And(UPWR,0x02)) // USB(HS1 : 0x480 /SS0 : 0x510)
    {
      \_SB.GP2N(40,0)         // Un-mask OC#
      \_SB.WTGP(13,1)
    }
    if(And(UPWR,0x04)) // USB(HS2 : 0x490 / SS1 : 0x520)
    {
      \_SB.GP2N(40,0)        // Un-mask OC#
      \_SB.WTGP(43,1)
    }
  } //if(LNotEqual(\UAMS,0x00)) // User not present
  } // End of DUAM
  }

//USB - END



// Serial IO Start

  //Power Resource for Audio Codec
Scope(\_SB.PCI0)
{
  PowerResource(PAUD, 0, 0) {
    
    Name(PSTA, One) // Physical Power Status of Codec 0 - OFF; 1-ON
    Name(ONTM, Zero) // 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON

    Name(_STA, One) // PowerResource Logical Status 0 - OFF; 1-ON 

    Method(_ON, 0){
      Store(One, _STA)        // Set Logocal power state
      PUAM() // Tansition Physical state to match current logical state
    } // End _ON

    Method(_OFF, 0){
      Store(Zero, _STA)    // Set the current power state
      PUAM() // Tansition Physical state to match current logical state
    } // End _OFF
    
    //  PUAM - Power Resource User Absent Mode for onboard Audio CODEC
    //  Arguments:
    //
    //  Uses:
    //      _STA - Variable updated by Power Resource _ON/_OFF methods
    //      \UAMS - Variable updated by GUAM method to show User absent/Present
    //      ONTM - Local variable to store ON time during Speculative ON
    // ______________________________
    // |  Inputs      |   Outputs    |
    // ______________________________
    // | _STA | \UAMS | GPIO | ONTM |
    // ______________________________
    // |   1  |   0   | ON   |   !0 |
    // |   1  |   !0  | ON   |   !0 |
    // |   0  |   0   | ON   |   !0 |
    // |   0  |   !0  | OFF  |   0  |
    // ______________________________

    Method(PUAM, 0, Serialized)
    {
      If (LAnd(LEqual(^_STA, Zero), LNotEqual(\UAMS, Zero))) { //New state = OFF (_STA ==0 && \UAMS != 0)
        If(LEqual(\_SB.RDGP(76), One)) { //skip if Power Resource is already in OFF
          //Device OFF
          // Drive GPIO 76 to Low
          \_SB.WTGP(76, Zero) // Remove CODEC Power
          Store(Zero, ^PSTA)
          Store(Zero, ^ONTM)
        }
      } Else { // New state = ON (_STA=1) or (_STA=0 and \UAMS=0)
        If(LNotEqual(^PSTA, One)) { //skip if Power Resource is already in ON
          SGON(76, One) // VR stagger GPIO76 to high to power on device
          Store(One, ^PSTA)
          Store(Timer(), ^ONTM) // Start the timer for this PR        
        }
      } // Turn power on
    } //PUAM
  } //PAUD
} //Scope(\_SB.PCI0)

If(LNot(HDAD)) { // Check Funtion disable register that gets updated depending on setup option
  Scope(\_SB.PCI0.HDEF) {

    Method(_PS0,0,Serialized)     // D0 Method for HD-A Controller
    {
      If(LEqual(\_SB.PCI0.PAUD.ONTM, Zero)){
        Return()
      }
     
      //
      // Make sure "D0 delay" (AUDD) delay is elapsed before returning _PS0
      // Local0: Elapse time since the _ON method
      // VRRD: VR Rampup Delay
      // AUDD: Time required for device to be ready after power on
      // Local1 = AUDD + VRRD: Need to incorporate VRRD since the _ON method no longer has VR Rampup Delay
      // So only need sleep for (Local1 - Local0), the amount of time remaining since the _ON method
      //
      Divide(Subtract(Timer(), \_SB.PCI0.PAUD.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
      Add(AUDD, VRRD, Local1) // Incorporate VR Rampup Delay 
      If(LLess(Local0, Local1)) { //Do not sleep if already past the delay requirement audio
        // Delay for power ramp
        Sleep(Subtract(Local1, Local0)) //Sleep (AUDD + VRRD - time elapsed) 
      }
    }
    Method(_PS3,0,Serialized)     // D3 Method for HD-A Controller(Dummy routine to support D3 state)
    {
    }

    Name(_PR0, Package(){\_SB.PCI0.PAUD})
  }
}// IF(HDAD)

// I2C0
Scope(\_SB.PCI0.I2C0) { // I2C0
    
    Method(_PR0, Zero, NotSerialized, 0, PkgObj)
    {
     Store(0, Local0) // BIT0 : ADSP Disabled BIT1 : SHUB Disabled
     If(LEqual(And(SDS0,0x01), 0x00)) { // Update Local0 BIT1 if SHUB disabled in setup
       Or(0x02,Local0,Local0)
     }

     If(LEqual(Local0,0x01)){ // Only SHUB enabled
       Return(Package(){\_SB.PCI0.I2C0.SHUB.PSEN,\_SB.PCI0.I2C1.PXTC})
     }
     Return(Package(){}) // Both SHUB and ADSP codec disabled
    }
    // D0 Method for I2C0 Controller(Called by _PS0)
    Method(PS0X,0,Serialized)
    {
      Sleep(I20D)  // If required add delay in controller to gurantee longest delaying device have time
      If(And(LEqual(RIC0,1),LEqual(\_SB.RDGP(44), 0x01))) { // Enabled in Setup and not in DFU mode
        // Drive GIO49 High - routed to Sensor HUB STOP mode
        \_SB.WTGP(49,1)
        Sleep(16)
      }
    }

    // D3 Method for I2C0 Controller(Called by _PS3)
    Method(PS3X,0,Serialized)
    {
      If(And(LEqual(RIC0,1),LEqual(\_SB.RDGP(44), 0x01))) { // Enabled in Setup and not in DFU mode
        // Drive GIO49 Low - routed to Sensor HUB STOP mode
        \_SB.WTGP(49,0)
        Sleep(16)
      }
    }
} // \_SB.PCI0.I2C0
Scope(\_SB.PCI0.I2C0.SHUB)
{
    // D0 Method Hook for I2C1 Controller(Called by _PS0)
    Method(_PS0,0,Serialized)
    {
      If(LEqual(\_SB.PCI0.I2C0.SHUB.PSEN.ONTM,Zero)){
        Return()
      }

      //
      // Make sure "D0 delay" (IC0D) delay is elapsed before returning _PS0
      // Local0: Elapse time since the _ON method
      // VRRD: VR Rampup Delay
      // IC0D: Time required for device to be ready after power on
      // Local1 = IC0D + VRRD: Need to incorporate VRRD since the _ON method no longer has VR Rampup Delay
      // So only need sleep for (Local1 - Local0), the amount of time remaining since the _ON method
      //
      Divide(Subtract(Timer(), \_SB.PCI0.I2C0.SHUB.PSEN.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
      Add(IC0D, VRRD, Local1) // Incorporate VR Rampup Delay 
      If(LLess(Local0, Local1)) //Do not sleep if already past the delay requirement 100ms for SensorHub
      {
        // Delay for power ramp
        Sleep(Subtract(Local1, Local0)) //Sleep (IC0D + VRRD - time elapsed) 
      }
    }
    Method(_PS3,0,Serialized)
    {
    }
    Method(_PR0, Zero, NotSerialized, 0, PkgObj)
    {
      If(And(LEqual(RIC0,1),LEqual(\_SB.RDGP(44), 0x01))) { // Enabled in Setup and not in DFU mode
        Return(Package(){PSEN,\_SB.PCI0.I2C1.PXTC})
      } Else {
        Return(Package(){})
      }
    }
    Method(_PR3, Zero, NotSerialized, 0, PkgObj)
    {
      If(And(LEqual(RIC0,1),LEqual(\_SB.RDGP(44), 0x01))) { // Enabled in Setup and not in DFU mode
        Return(Package(){PSEN,\_SB.PCI0.I2C1.PXTC})
      } Else {
        Return(Package(){})
      }
    }

  PowerResource(PSEN, 0, 0) {
    Name(ONTM, Zero) // On Timer

    Method(_STA){
      If(LEqual(SHSB, 0)){
        If(LEqual(\_SB.RDGP(46),1)) { //GPIO46 NOTE: 1=> ON; 0=> OFF
          Return(0x01)
        }
     }Else{
        If(LEqual(\_SB.RDGP(70),1)) { //GPIO70 NOTE: 1=> ON; 0=> OFF
          Return(0x01)
        }
     }
     Return(0x00)
   }

    Method(_ON, 0){
     If(And(LEqual(RIC0,1),LEqual(\_SB.RDGP(44), 0x01))) { // Enabled in Setup and not in DFU mode
        // Drive GPIO 46 High
      If(LEqual(SHSB, 0)){
        If(LEqual(SGON(46, One), One)) { // VR stagger GPIO46 to high to power on device
          Store(Timer(), ^ONTM) // Start the timer
          \_SB.GP2B(14, One)  // Enable int line from sensor hub. GPIO14 is PIRQM(IOAPIC ln28)
        }
      }Else{
        If(LEqual(SGON(70, One), One)) { // VR stagger GPIO70 to high to power on device
          Store(Timer(), ^ONTM) // Start the timer
          \_SB.GP2B(14, One)  // Enable int line from sensor hub. GPIO14 is PIRQM(IOAPIC ln28)
        }
      }
     } // End if - Enabled in Setup and not in DFU mode
    } // End _ON

    Method(_OFF, 0){
     If(And(LEqual(RIC0,1),LEqual(\_SB.RDGP(44), 0x01))) { // Enabled in Setup and not in DFU mode    
        // Mask interrupt from Sensorhub GPIPIRQ2IOXAPIC: GPI PIRQ[X:I] to IOxAPIC[39:24] Enable
        \_SB.GP2B(14, Zero) // GPIO14 is PIRQM(IOAPIC ln28)
        If(LEqual(SHSB, 0)){
          \_SB.WTGP(46, Zero)  // Remove Device Power
        }Else{
          \_SB.WTGP(70, Zero)
        }     
        Store(Zero, ^ONTM)
      Store(Zero , ^ONTM)      
     }// End if - Enabled in Setup and not in DFU mode
    } // End _OFF    
  } //PSEN
} // Scope(SHUB)

  //Merge TouchPanel/Touch Pad Power control - I2C Sensors Need to Turn ON/OFF at same time
  Scope(\_SB.PCI0.I2C1){

    Method(PS0X,0,Serialized)
    {
      // If required add delay in controller to gurantee longest delaying device have time
      Sleep(I21D)
    }

    Method(PS3X,0,Serialized)
    {
    }

    PowerResource(PXTC, 0, 0){ // power rail for Touch Panel and Touchpad GPIO 89

      Name(ONTM, 0) // 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON
      Method(_STA){
        If(LEqual(\_SB.RDGP(89),1)) { //GPIO89 NOTE: 1=> ON; 0=> OFF
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(_ON, 0)
      {
       If(LOr(LEqual(\_SB.RDGP(89),0), LEqual(\_SB.RDGP(52),0))) { //GPIO89 NOTE: 1=> ON; 0=> OFF
        \_SB.WTGP(52,1) //DeAssert GPIO RST
        SGON(89, One) // VR stagger GPIO89 to high to power on device
        Store(Timer(), ^ONTM)
        //Enable int line from touch screen and Touch pad
        \_SB.GP2A(50, One) //TPL0
        If(LAnd(LNotEqual(And(SDS1,0x0800), 0x0800),LNotEqual(And(SDS1,0x1000), 0x1000))) {    //Precision Enabled
          \_SB.GP2A(55, One) //TPD0/1
        }
       }
      }// End _ON

      Method(_OFF, 0)
      {
        //mask interrupt from Touch Screen and Touch Pad GPIPIRQ2IOXAPIC: GPI PIRQ[X:I] to IOxAPIC[39:24] Enable
        \_SB.GP2A(50, Zero) //TPL0
        If(LAnd(LNotEqual(And(SDS1,0x0800), 0x0800),LNotEqual(And(SDS1,0x1000), 0x1000))) { //Precision Enabled
          \_SB.GP2A(55, Zero) //TPD0/1
        }
        \_SB.WTGP(89,0) // Drive GPIO89 to remove power for Touchpad and TouchPanel
        \_SB.WTGP(52,0) //GPIO RST to Touch Panel
        Store(Zero , ^ONTM)
      }// End _OFF
    } // End PXTC

    Scope(TPL0){
      Method(_PS0,0,Serialized)
      {
        If(LEqual(\_SB.PCI0.I2C1.PXTC.ONTM,Zero)) {
          Return()
        }

        //
        // Make sure "D0 delay" (IC1D) delay is elapsed before returning _PS0
        // Local0: Elapse time since the _ON method
        // VRRD: VR Rampup Delay
        // IC1D: Time required for device to be ready after power on
        // Local1 = IC1D + VRRD: Need to incorporate VRRD since the _ON method no longer has VR Rampup Delay
        // So only need sleep for (Local1 - Local0), the amount of time remaining since the _ON method
        //
        Divide(Subtract(Timer(), \_SB.PCI0.I2C1.PXTC.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
        Add(IC1D, VRRD, Local1) // Incorporate VR Rampup Delay 
        If(LLess(Local0, Local1)) //Do not sleep if already past the delay requirement 100ms for Touchpanel
        {
          // Delay for power ramp
          Sleep(Subtract(Local1, Local0)) //Sleep (IC1D + VRRD - time elapsed)
        }
      }
      Method(_PS3,0,Serialized)
      {
      }      
      Method(_PR0, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x01), 0x01))
        {
          Return(Package(){\_SB.PCI0.I2C0.SHUB.PSEN,PXTC})
        } Else {
          Return(Package() {})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x01), 0x01))
        {
          Return(Package(){\_SB.PCI0.I2C0.SHUB.PSEN,PXTC})
        } Else {
          Return(Package() {})
        }        
      }
    } // End of Scope(TPL0)
    Scope(TPD1){
      Method(_PS0,0,Serialized)
      {
        If(LEqual(\_SB.PCI0.I2C1.PXTC.ONTM,Zero)) {
          Return()
        }
        //
        // Make sure "D0 delay" (IC1S) delay is elapsed before returning _PS0
        // Local0: Elapse time since the _ON method
        // VRRD: VR Rampup Delay
        // IC1S: Time required for device to be ready after power on
        // Local1 = IC1S + VRRD: Need to incorporate VRRD since the _ON method no longer has VR Rampup Delay
        // So only need sleep for (Local1 - Local0), the amount of time remaining since the _ON method
        //
        Divide(Subtract(Timer(), \_SB.PCI0.I2C1.PXTC.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
        Add(IC1S, VRRD, Local1) // Incorporate VR Rampup Delay 
        If(LLess(Local0, Local1)) //Do not sleep if already past the delay requirement 100ms for Touchpanel
        {
          // Delay for power ramp
          Sleep(Subtract(Local1, Local0)) //Sleep (IC1S + VRRD - time elapsed)
        }
      }    
      Method(_PS3,0,Serialized)
      {
      }
      Method(_PR0, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x08), 0x08)) {
            Return(Package(){\_SB.PCI0.I2C0.SHUB.PSEN,PXTC})
        } Else {
          Return(Package() {})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x08), 0x08)) {
            Return(Package(){\_SB.PCI0.I2C0.SHUB.PSEN,PXTC})
        } Else {
          Return(Package() {})
        }
      }
    }// End of Scope(TPD1)

    //------------------------
    //  Elantech Precision touchpad
    //------------------------
    Scope(TPD7){

      //
      // Delay for Touchpanel & TouchPad - Both share same I2C bus
      //
      Method(PS0X,0,Serialized)
      {
        If(LEqual(\_SB.PCI0.I2C1.PXTC.ONTM,Zero)) {
          Return()
        }
        //
        // Make sure "D0 delay" (IC1S) delay is elapsed before returning _PS0
        // Local0: Elapse time since the _ON method
        // VRRD: VR Rampup Delay
        // IC1S: Time required for device to be ready after power on
        // Local1 = IC1S + VRRD: Need to incorporate VRRD since the _ON method no longer has VR Rampup Delay
        // So only need sleep for (Local1 - Local0), the amount of time remaining since the _ON method
        //
        Divide(Subtract(Timer(), \_SB.PCI0.I2C1.PXTC.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
        Add(IC1S, VRRD, Local1) // Incorporate VR Rampup Delay 
        If(LLess(Local0, Local1)) //Do not sleep if already past the delay requirement 100ms for Touchpanel
        {
          // Delay for power ramp
          Sleep(Subtract(Local1, Local0)) //Sleep (IC1S + VRRD - time elapsed)
        }
      }

      Method(_PR0, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x0800), 0x0800)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x0800), 0x0800)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
    }// End of Scope(TPD7)
    
    //------------------------
    //  Synaptics Precision touchpad
    //------------------------
    Scope(TPD8){

      //
      // Delay for Touchpanel & TouchPad - Both share same I2C bus
      //
      Method(PS0X,0,Serialized)
      {
        If(LEqual(\_SB.PCI0.I2C1.PXTC.ONTM,Zero)) {
          Return()
        }
        //
        // Make sure "D0 delay" (IC1S) delay is elapsed before returning _PS0
        // Local0: Elapse time since the _ON method
        // VRRD: VR Rampup Delay
        // IC1S: Time required for device to be ready after power on
        // Local1 = IC1S + VRRD: Need to incorporate VRRD since the _ON method no longer has VR Rampup Delay
        // So only need sleep for (Local1 - Local0), the amount of time remaining since the _ON method
        //
        Divide(Subtract(Timer(), \_SB.PCI0.I2C1.PXTC.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
        Add(IC1S, VRRD, Local1) // Incorporate VR Rampup Delay 
        If(LLess(Local0, Local1)) //Do not sleep if already past the delay requirement 100ms for Touchpanel
        {
          // Delay for power ramp
          Sleep(Subtract(Local1, Local0)) //Sleep (IC1S + VRRD - time elapsed)
        }
      }

      Method(_PR0, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x1000), 0x1000)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x1000), 0x1000)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
    }// End of Scope(TPD8)
 }//  Scope(\_SB.PCI0.I2C1)

//Serial IO End

  //SDIO Reset support
  Scope(\_SB.PCI0.SDHC)
  {
    // D0 Method for SDHC
    Method(PS0X,0,Serialized)
    {
      // Drive GPIO77 High - 1: No GPIO RST to on board mPCIe/NGFF_WIFI
      \_SB.WTGP(77,1)
    }
  }

  Scope(\_SB.PCI0.SDHC.WI01)
  {
    // D0 Method for WiFi
    Method(PS0X,0,Serialized)
    {
      // Drive GPIO77 High - 1: No GPIO RST to on board mPCIe/NGFF_WIFI
      \_SB.WTGP(77,1)
    }

    // D3 Method for WiFi
    Method(PS3X,0,Serialized)
    {
      // Drive GPIO77 Low - 0: GPIO RST to on board mPCIe/NGFF_WIFI
      \_SB.WTGP(77,0)
    }
  } //\_SB.PCI0.SDHC.WI01

} //If(LAnd(LEqual(BID,BHB),LEqual(RTD3,1)))
} // End SSDT
