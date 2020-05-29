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
    "Ult0Rtd3",
    0x1000
    )
{
#define ULT0RTD3 1
Include ("RTD3Common.asl")
#undef ULT0RTD3

External(PEBS, IntObj)        // PCI Express BAR address
External(BWT1, IntObj)        // WhitTipMountain1
External(BSPC, IntObj)        // SawToothPeak

External(\_SB.PCI0.I2C0.ACD0, DeviceObj)
External(\_SB.PCI0.I2C0.ACD1, DeviceObj)
External(\_SB.PCI0.I2C0.ACD2, DeviceObj)
External(\_SB.PCI0.I2C1.TPL1, DeviceObj) // Touch panel
External(\_SB.PCI0.I2C1.TPL2, DeviceObj) // NTRIG Digitizer Touch Panel
External(\_SB.PCI0.I2C1.TPD2, DeviceObj) // Alps touchpad
External(\_SB.PCI0.I2C1.TPL3, DeviceObj) // EETI Touch Panel
External(\_SB.PCI0.I2C1.TPD3, DeviceObj) // Cypress touchpad
External(\_SB.PCI0.I2C1.TPD7, DeviceObj) // Precision Elantech touchpad
External(\_SB.PCI0.I2C1.TPD8, DeviceObj) // Precision Synaptics touchpad
External(\_SB.PCI0.SDHC, DeviceObj) //SDIO
External(\_SB.PCI0.SDHC.WI01, DeviceObj) //SDIO Wifi
External(\_SB.PCI0.XHC.RHUB.HS10, DeviceObj) //xHCI SSP port 1
External(S0ID)
External(\_SB.PCI0.XHC.RHUB.HS11, DeviceObj) //xHCI SSP port 2
External(\_SB.PCI0.XHC.RHUB.HS04, DeviceObj) //EHCI HSP port 4

External(ADSD, FieldUnitObj)
External(CODS, IntObj)
External(\GO17)


External(DSPD)
External(RWAG, IntObj)

If(LAnd(LOr(LEqual(BID,BWT1), LEqual(BID,BSPC)),LEqual(RTD3,1))) {

If(LEqual(RWAG,1)) { // this enables board W/A for RTD3 on slot 1
  //
  // PCIe RTD3 - for slot#1
  //
  Scope(\_SB.PCI0.RP01)
  {
    Name(SLOT, 1)
    Name(RSTG, 77)  // reset GPIO
    Name(PWRG, 83)  // power GPIO
    Name(RONP, 1)   // reset on polarity
    Name(PONP, 0)   // power on polarity
    Name(NGFF, 1)   // Is Slot NGFF form factor 1- Yes 0- No
    Name(SCLK, 0x1) // Source Clock Enable Bit Location(BIT0)
    Include("RTD3Pcie.asl")
  }
}

If(LEqual(RWAG,6)) { // this enables board W/A for RTD3 on slot 6
  //
  // PCIe RTD3 - for slot#6
  //
  Scope(\_SB.PCI0.RP06)
  {
    Name(SLOT, 6)
    Name(RSTG, 77)  // reset GPIO
    Name(PWRG, 83)  // power GPIO
    Name(RONP, 1)   // reset on polarity
    Name(PONP, 0)   // power on polarity
    Name(NGFF, 0)   // Is Slot NGFF form factor 1- Yes 0- No
    Name(SCLK, 0x2) // Source Clock Enable Bit Location(BIT1)
    Include("RTD3Pcie.asl")
  }
}

  //
  // PCIe RTD3 - for slot#3
  //
  Scope(\_SB.PCI0.RP03)
  {
    Name(SLOT, 3)
    Name(RSTG, 59)  // reset GPIO
    Name(PWRG, 60)  // power GPIO
    Name(RONP, 1)   // reset on polarity
    Name(PONP, 0)   // power on polarity
    Name(NGFF, 0)   // Is Slot NGFF form factor 1- Yes 0- No
    Name(SCLK, 0x4) // Source Clock Enable Bit Location(BIT2)
    Include("RTD3Pcie.asl")
  }

  //
  // PCIe RTD3 - for slot#4
  //
  Scope(\_SB.PCI0.RP04)
  {
    Name(SLOT, 4)
    Name(RSTG, 56)  // reset GPIO
    Name(PWRG, 57)  // power GPIO
    Name(RONP, 1)   // reset on polarity
    Name(PONP, 0)   // power on polarity
    Name(NGFF, 0)   // Is Slot NGFF form factor 1- Yes 0- No
    Name(SCLK, 0x8) // Source Clock Enable Bit Location(BIT3)
    Include("RTD3Pcie.asl")
  }

  //
  // PCIe RTD3 - for slot#5
  //
  Scope(\_SB.PCI0.RP05)
  {
    Name(SLOT, 5)
    Name(RSTG, 48)   // reset GPIO
    Name(PWRG, 84)   // power GPIO
    Name(RONP, 1)    // reset on polarity
    Name(PONP, 0)    // power on polarity
    Name(NGFF, 0)    // Is Slot NGFF form factor 1- Yes 0- No
    Name(SCLK, 0x10) // Source Clock Enable Bit Location(BIT4)
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

    Scope(PRT1) {
      Name(UDSM, 0) // Use _DSM for ZPODD control. 0 = Disabled; 1 = Enabled

      //
      // _DSM Device Specific Method supporting SATA ZPODD function
      //
      // Arg0: UUID Unique function identifier
      // Arg1: Integer Revision Level
      // Arg2: Integer Function Index
      // Arg3: Package Parameters
      //
      Method (_DSM, 4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      if(LNot(And(RCG0,1))){
        Return(0) // HDD do nothing
      }
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
              if(LEqual(ToInteger(Arg1),1)){ // Check Revision
                Return (Buffer () {0xF})
              }
            }
            Case (1) { // Enable ZPODD feature
              // Function 1: Enable Power OFF/ON. Enables the Power Off/ON hardware in the system.
              // When called, the BIOS is informed that the host software is using this interface. 
              If(LEqual(\_SB.PCI0.SAT0.PRT1.UDSM, Zero)){ //if Ownership not acquired
                Store (1, \_SB.PCI0.SAT0.PRT1.UDSM)  // Indicate ZPODD _DSM control enabled
              }
              Return (1) // Enable Power ON/OFF
            }
            Case (2) {
              \_SB.WTGP(86,1) // Power Off the device
              Store(0,\GO17) // Enable GPIO86
              Return (1)
           }
          Case (3) {
            \_SB.WTGP(86,0) // Power ON Device
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

    Name(_PR0, Package(){P1PR})
    Name(_PR3, Package(){P1PR})

    Name(WKEN, 0) // Device on this port wake enabled?

    Method(_S0W, 0)
    {
      if(LNot(And(RCG0,1))){
      }Else{
        // PMEs can be generated from D3cold
        If(And(\_SB.OSCO, 0x04))
        {
          Return(4) // OS comprehends D3cold, as described via \_SB._OSC
        }Else{
          Return(3)
        }
      }
    } // End _S0W

    Method(_DSW, 3)
    {
      if(LNot(And(RCG0,1))){
        Return()
      }
      If(Arg1)
      { 
        Store(1,\GO17)
        Store(0, WKEN)
      }Else{  // Staying in S0
        If(LAnd(Arg0, Arg2)) // Exiting D0 and arming for wake
        { // Set WKEN to allow _OFF to enable the wake event
          Store(1, WKEN)
        } Else { // Clear WKEN to allow _OFF to enable the wake event, 
               // either because staying in D0 or disabling wake 
          Store(0, WKEN)
        }
      }  
    } // End _DSW
    
    
    PowerResource(P1PR, 0, 0)
    {
      Name(OFTM, Zero) // Time returned by Timer() when the Power resource was turned OFF

      Method(_STA)
      {
        if(LNot(And(RCG0,1))){
          If(LEqual(\_SB.RDGP(90),1)) { //GPIO90
            Return(0x00)
          } Else {
            Return(0x01)
          }
        }Else{
          // Read ZPODD Status
          if(LEqual(RDGP(86),0)){
            Return(0x01)
          }Else{
            Return(0x00)
          }
        }
      }
      
      Method(_ON, 0)
      {
        If(LNotEqual(^OFTM, Zero)) { // if OFTM != 0 => Disk was turned OFF by asl
          Divide(Subtract(Timer(), ^OFTM), 10000, , Local0) //Store Elapsed time in ms
          Store(Zero, ^OFTM) // Reset OFTM to zero to indicate minimum 50ms requirement does not apply when _ON called next time
          If(LLess(Local0, 50)) // Do not sleep if already past the delay requirement
          {
            Sleep(Subtract(50, Local0)) // Sleep 50ms - time elapsed
          }
        }

        if(LNot(And(RCG0,1))){
          SGON(90, Zero) // VR stagger GPIO90 to low to power on device
        }Else{    
          
            SGON(86, Zero) // VR stagger GPIO86 to low to power on device
          }
        }
      
      
      Method(_OFF, 0)
      {
        If(LNot(And(RCG0,1))){
          Add(\_SB.PCI0.SAT0.MBR6,0x198 ,Local0)
          OperationRegion(PSTS, SystemMemory, Local0, 0x18) 
          Field(PSTS, DWordAcc, NoLock, Preserve) 
          {
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
          // if S0Ix enabled
          If(LEqual(S0ID, 1))
          {          
            If(LOr(LEqual(SDET, 1), LEqual(SDET, 3)))  //Offline flow only if Device detected and Phy not offline
            {  
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
          } // if S0Ix enabled
        }Else{
          
          \_SB.WTGP(86,1) // Drive GPIO86 to low to power off device.
          Store(0,\GO17) // Enable ZPODD SATA_ODD_DA_N to generate SCI
          }
        
        Store(Timer(), ^OFTM) // Store time when Disk turned OFF(non-zero OFTM indicate minimum 50ms requirement does apply when _ON called next time)
      }
    } // End P1PR
  } //End PRT1
    
    // PRT2 . Controlled by EC pin (PF6/PWMU4A/ExCTS).
    Scope(PRT2) {  // Port 2- Cable Connect power

      PowerResource(P2PR, 0, 0){
        Name(OFTM, Zero) // Time returned by Timer() when the Power resource was turned OFF

        Method(_STA){
          If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) { // Read SATA_CABLE_PWR_EN from EC
            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.SPT2)), Local0)
          } Else {
            Store(1, Local0) //This FET is ON during POST. It cannot be turned OFF until EC driver is loaded
          }
          If(LEqual(Local0,0)) { // Read SATA_CABLE_PWR_EN from EC
            Return(0x00)
          } Else {
            Return(0x01)
          }
          
          Return(0x01)
        }
        Method(_ON, 0)
        {
          If(LNotEqual(^OFTM, Zero)) { // if OFTM != 0 => Disk as turned OFF by asl
            Divide(Subtract(Timer(), ^OFTM), 10000, , Local0) //Store Elapsed time in ms
            Store(Zero, ^OFTM) // Reset OFTM to zero to indicate minimum 50ms requirement does not apply when _ON called next time
            If(LLess(Local0, 50)) // Do not sleep if already past the delay requirement
            {
              Sleep(Subtract(50, Local0)) // Sleep 50ms - time elapsed
            }
          }

          //SET SATA_CABLE_PWR_EN  and Send EC UpdateDevPwr - 0x2C
          If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) { // Check If EC driver loaded
            //
            // Special case for EC driven device power on using VR staggering (SGON is for GPIO driven device power on)
            // Marked with "//VR Staggering" at the end of line.
            //                                                                                                                  //VR Staggering
            // Check for VR staggering                                                                                          //VR Staggering
            //                                                                                                                  //VR Staggering
            Divide(Subtract(Timer(), \LONT), 10000, , Local0)   //Store Elapsed time in ms, ignore remainder                    //VR Staggering
            If(LLess(Local0, \VRSD)) {                          //Do not sleep if already past the delay requirement            //VR Staggering
              // Delay for power ramp using Timer Based Sleep                                                                   //VR Staggering
              Sleep(Subtract(\VRSD, Local0))                                                                                    //VR Staggering
            }                                                                                                                   //VR Staggering
            
            \_SB.PCI0.LPCB.H_EC.ECWT(One, RefOf(\_SB.PCI0.LPCB.H_EC.SPT2))
            \_SB.PCI0.LPCB.H_EC.ECMD (0x2E)

            Store(Timer(), \LONT) // Update the global Last ON Method Timer                                                     //VR Staggering
          }
        }
        Method(_OFF, 0)
        {
          If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) { // Check If EC driver loaded
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
              //Clear SATA_CABLE_PWR_EN  and Send EC UpdateDevPwr - 0x2C
              \_SB.PCI0.LPCB.H_EC.ECWT(Zero, RefOf(\_SB.PCI0.LPCB.H_EC.SPT2))
              \_SB.PCI0.LPCB.H_EC.ECMD (0x2E)
          }
          Store(Timer(), ^OFTM) // Store time when Disk turned OFF(non-zero OFTM indicate minimum 50ms requirement does apply when _ON called next time)
        }
      } // End P2PR

      Name(_PR0, Package(){P2PR})
      Name(_PR3, Package(){P2PR})

    } //End PRT2

    // PRT3 
      Scope(PRT3) {

// RCG0 : BIT2BIT3 : 00 - Disabled 01 - 1.5Minicard(GPIO83) 10:3.3Minicard(GPIO85)
      PowerResource(P3PR, 0, 0){
        Method(_STA){
          if(LEqual(And(RCG0,0xC),0x04)){ // 1.5Minicard GPIO83
            Return(\_SB.RDGP(83))
          }
          if(LEqual(And(RCG0,0xC),0x08)){ // 3.3Minicard GPIO85
            Return(\_SB.RDGP(85))
          }
            Return(0x00) // Disabled
        }
        Method(_ON, 0)
        {
          if(LEqual(And(RCG0,0xC),0x04)){ // 1.5Minicard GPIO83
            SGON(83, One) // VR stagger GPIO83 to high to power on device
          }
          if(LEqual(And(RCG0,0xC),0x08)){ // 3.3Minicard GPIO85
            SGON(85, One) // VR stagger GPIO85 to high to power on device
          }
        }
        Method(_OFF, 0)
        {
            Add(\_SB.PCI0.SAT0.MBR6,0x298 ,Local0)
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

          if(LEqual(And(RCG0,0xC),0x04)){ // 1.5Minicard GPIO83
            \_SB.WTGP(83,0)
            Sleep(16) // Delay for power ramp
          }
          if(LEqual(And(RCG0,0xC),0x08)){ // 3.3Minicard GPIO85
            \_SB.WTGP(85,0)
            Sleep(16) // Delay for power ramp
          }
        }
      } // End P2PR

      Name(_PR0, Package(){P3PR})
      Name(_PR3, Package(){P3PR})

    } //End PRT3


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

    Method(_S0W, 0)
    { // PMEs can be generated from D3(hot)
        Return(3)
    } // End _S0W
  }

  Scope(\_SB.PCI0.XHC.RHUB){ //USB XHCI RHUB
    Method(_PS0,0,Serialized)
    {
      Store(0x00,USPP)
    }
    Method(_PS2,0,Serialized)
    {
    
      OperationRegion (XHCM, SystemMemory, And(MEMB,0xFFFF0000), 0x600)
      Field (XHCM, DWordAcc, NoLock, Preserve)
      {
        Offset(0x2),
        XHCV,16,
        Offset(0x480),
        HP01, 1, // HS port 1
        Offset(0x490),
        HP02, 1, // HS port 2
        Offset(0x510),
        SP00, 1, // SS port 0
        Offset(0x520),
        SP01, 1, // SS port 1
      }
      If(LEqual(XHCV,0xFFFF)) // Controller in D3Hot(MEM_BASE not decoded)
      {
       Return()
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
    } // End of PS3 method
  }

If(LNotEqual(And(XHPR,0xF),0x0)){
  Scope(\) {
    PowerResource(PX01, 0, 0){ // power rail for USB3.0 ports 1 - GPIO 13
      Method(_STA){
        If(LEqual(\_SB.RDGP(13),1)) { //GPIO13
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(_ON, 0)
      { // Turn on
        SGON(13, One) // VR stagger GPIO13 to high to power on device
      }

      Method(_OFF, 0)
      { // Turn off
        \_SB.WTGP(13,0)
      }
    } // End PX01
  }
}
If(LNotEqual(And(XHPR,0xF0),0x0)){
  Scope(\) {
    PowerResource(PX02, 0, 0){ // power rail for USB3.0 ports 2 - GPIO 43
      Method(_STA){
        If(LEqual(\_SB.RDGP(43),1)) { //GPIO43
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(_ON, 0)
      { // Turn on
        SGON(43, One) // VR stagger GPIO43 to high to power on device
      }

      Method(_OFF, 0)
      { // Turn off
        \_SB.WTGP(43,0)
      }
    } // End PX02

  }
}

if(LEqual(And(RCG0,2),2))
{
  // USB Camera RTD3 support is enabled
  Scope(\_SB.PCI0.XHC.RHUB.HS04){
      PowerResource(PX04, 0, 0){ // power rail for USB2.0 Port 4 - GPIO 25
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
    } // End PX04
    
    Method(_PR0, Zero, NotSerialized, 0, PkgObj)
    {
      Return(Package(){\_SB.PCI0.XHC.RHUB.HS04.PX04})
    } // End of Method _PR0
    Method(_PR2, Zero, NotSerialized, 0, PkgObj)
    {
      Return(Package(){\_SB.PCI0.XHC.RHUB.HS04.PX04})
    } // End of Method _PR2
    Method(_PR3, Zero, NotSerialized, 0, PkgObj)
    {
      Return(Package(){\_SB.PCI0.XHC.RHUB.HS04.PX04})
    } // End of Method _PR3
  } // Scope(\_SB.PCI0.XHC.RHUB.HS04)
}

If(LEqual(And(XHPR,0xF),0x01))
{
  Scope(\_SB.PCI0.XHC.RHUB.HS01){
    Name(_PR0, Package(){\PX01})
    Name(_PR2, Package(){\PX01})
    Name(_PR3, Package(){\PX01})
  }
}

If(LEqual(And(XHPR,0xF0),0x10)){
  Scope(\_SB.PCI0.XHC.RHUB.HS02){
    Name(_PR0, Package(){\PX02})
    Name(_PR2, Package(){\PX02})
    Name(_PR3, Package(){\PX02})
  }
}

If(LEqual(And(XHPR,0xF),0x02)){
  Scope(\_SB.PCI0.XHC.RHUB.SSP1){
    Name(_PR0, Package(){\PX01})
    Name(_PR2, Package(){\PX01})
    Name(_PR3, Package(){\PX01})
  } //  End of Scope(\_SB.PCI0.XHC.RHUB.SSP1)
}  

If(LEqual(And(XHPR,0xF0),0x20)){
  Scope(\_SB.PCI0.XHC.RHUB.SSP2){
    Name(_PR0, Package(){\PX02})
    Name(_PR2, Package(){\PX02})
    Name(_PR3, Package(){\PX02})  
  } //  End of Scope(\_SB.PCI0.XHC.RHUB.SSP2)
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
          SGON(76, One) // VR stagger GPIO76 to High to power on device
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
        // Delay for device init
        Sleep(Subtract(Local1, Local0)) //Sleep (AUDD + VRRD - time elapsed) 
      }
    }
    Method(_PS3,0,Serialized)     // D3 Method for HD-A Controller(Dummy routine to support D3 state)
    {
    }

    Name(_PR0, Package(){\_SB.PCI0.PAUD})
  }
  }// IF(HDAD)

// ADSP device
if(LNot(ADSD)){ // Check Funtion disable register that gets updated depending on setup option
  Scope(\_SB.PCI0.ADSP) {

    // D0 Method for ADSP Controller
    Method(_PS0,0,Serialized)      // D0 Method for ADSP Controller
    {
      
      If(LEqual(\_SB.PCI0.PAUD.ONTM, Zero)) {
        Return()
      }
     
      //
      // Make sure "D0 delay" (DSPD) delay is elapsed before returning _PS0
      // Local0: Elapse time since the _ON method
      // VRRD: VR Rampup Delay
      // DSPD: Time required for device to be ready after power on
      // Local1 = DSPD + VRRD: Need to incorporate VRRD since the _ON method no longer has VR Rampup Delay
      // So only need sleep for (Local1 - Local0), the amount of time remaining since the _ON method
      //
      Divide(Subtract(Timer(), \_SB.PCI0.PAUD.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
      Add(DSPD, VRRD, Local1) // Incorporate VR Rampup Delay 
      If(LLess(Local0, Local1)) { //Do not sleep if already past the delay requirement audio
        // Delay for device init
        Sleep(Subtract(Local1, Local0)) //Sleep (DSPD + VRRD - time elapsed) 
      }
    }
    Method(_PS3,0,Serialized)     // D3 Method for ADSP Controller(Dummy routine to support D3 state)
    {
    }

    Name(_PR0, Package(){\_SB.PCI0.PAUD})
  }
}// if(ADSD)

// I2C0
Scope(\_SB.PCI0.I2C0) { // I2C0
    
    Method(_PR0, Zero, NotSerialized, 0, PkgObj)
    {
     Store(0, Local0) // BIT0 : ADSP Disabled BIT1 : SHUB Disabled
     If (LEqual(ADSD,1)){ // Update Local0 BIT0 if ADSP disabled in RCBA function disable register(implies disabled in setup)
       Or(0x01,Local0,Local0)
     }
     If(LEqual(And(SDS0,0x01), 0x00)) { // Update Local0 BIT1 if SHUB disabled in setup
       Or(0x02,Local0,Local0)
     }

     If(LEqual(Local0,0x0)){ // Both SHUB and ADSP enabled
       Return(Package(){\_SB.PCI0.I2C0.SHUB.PSEN,\_SB.PCI0.I2C1.PXTC,\_SB.PCI0.PAUD})
     }     
     If(LEqual(Local0,0x01)){ // Only SHUB enabled
       Return(Package(){\_SB.PCI0.I2C0.SHUB.PSEN,\_SB.PCI0.I2C1.PXTC})
     }
     If(LEqual(Local0,0x02)){ // Only ADSP codec enabled
       Return(Package(){\_SB.PCI0.PAUD})
     }
     Return(Package(){}) // Both SHUB and ADSP codec disabled
    }
    // D0 Method for I2C0 Controller(Called by _PS0)
    Method(PS0X,0,Serialized)
    {
      Sleep(I20D)  // If required add delay in controller to gurantee longest delaying device have time
      // Drive GIO49 High - routed to Sensor HUB STOP mode
      \_SB.WTGP(49,1)
      Sleep(16)
    }

    // D3 Method for I2C0 Controller(Called by _PS3)
    Method(PS3X,0,Serialized)
    {

      // Drive GIO49 Low - routed to Sensor HUB STOP mode
      \_SB.WTGP(49,0)
      Sleep(16)
    }
} // \_SB.PCI0.I2C0

If (AND(LEqual(CODS, 0),LNot(ADSD))){ // CODEC 0 : RealTek
 Scope(\_SB.PCI0.I2C0.ACD0)
 {
   Name(_PR0,Package(){\_SB.PCI0.PAUD})
 }
}

If (AND(LEqual(CODS, 1),LNot(ADSD))){ // CODEC 1 : Cirrus
 Scope(\_SB.PCI0.I2C0.ACD1)
 {
   Name(_PR0,Package(){\_SB.PCI0.PAUD})
 }
}

If (AND(LEqual(CODS, 2),LNot(ADSD))){ // CODEC 2 : Idt
 Scope(\_SB.PCI0.I2C0.ACD2)
 {
   Name(_PR0,Package(){\_SB.PCI0.PAUD})
 }
}

Scope(\_SB.PCI0.I2C0.SHUB)
{
    // D0 Method Hook for I2C1 Controller(Called by _PS0)
    Method(_PS0,0,Serialized)
    {
      If(LEqual(\_SB.PCI0.I2C0.SHUB.PSEN.ONTM,Zero)) {
        Return()
      }
    
      // Make sure "D0 delay" (IC0D) delay is elapsed before returning _PS0
      Divide(Subtract(Timer(), \_SB.PCI0.I2C0.SHUB.PSEN.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
      If(LLess(Local0, IC0D)) //Do not sleep if already past the delay requirement 100ms for SensorHub
      {
        // Delay for power ramp
        Sleep(Subtract(IC0D, Local0)) //Sleep (100 - time elapsed) 
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
        If(LEqual(\_SB.RDGP(46),1)) { //GPIO89 NOTE: 1=> ON; 0=> OFF
          Return(0x01)
        }
     }Else{
        If(LEqual(\_SB.RDGP(70),1)) { //GPIO89 NOTE: 1=> ON; 0=> OFF
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
     } // End if - Enabled in Setup and not in DFU mode
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
        If(LOr(LEqual(And(SDS1,0x0800), 0x0800),LEqual(And(SDS1,0x1000), 0x1000))){    //Precision Enabled
          If(LEqual(\_SB.RDGP(89),1)) { //GPIO89 NOTE: 1=> ON; 0=> OFF
            Return(0x01)
          } Else {
            Return(0x00)
          }
        } Else{
          If(LAnd(LEqual(\_SB.RDGP(88),1), LEqual(\_SB.RDGP(89),1))) { //GPIO88/89 NOTE: 1=> ON; 0=> OFF
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
      }
      Method(_ON, 0)
      { // Turn on
        If(LOr(LEqual(And(SDS1,0x0800), 0x0800),LEqual(And(SDS1,0x1000), 0x1000))){    //Precision Enabled
          If(LOr(LEqual(\_SB.RDGP(89),0), LEqual(\_SB.RDGP(52),0))) { //GPIO88 NOTE: 1=> ON; 0=> OFF
            \_SB.WTGP(52,1) //DeAssert GPIO RST
            SGON(89, One) // VR stagger GPIO89 to high to power on device
            Store(Timer(), ONTM)
            //Enable int line from touch screen
            \_SB.GP2A(50, One) //TPL0
          }
        }Else {
          If(LOr(LOr(LEqual(\_SB.RDGP(88),0), LEqual(\_SB.RDGP(89),0)), LEqual(\_SB.RDGP(52),0))) { //GPIO88/89 NOTE: 1=> ON; 0=> OFF
            \_SB.WTGP(52,1) //DeAssert GPIO RST
            SGON(88, One) // VR stagger GPIO88 to high to power on device
            SGON(89, One) // VR stagger GPIO89 to high to power on device
            Store(Timer(), ONTM)
            //Enable int line from touch screen and Touch pad
            \_SB.GP2A(50, One) //TPL0
            \_SB.GP2A(55, One) //TPD0/1
          }
        }
      }

      Method(_OFF, 0)
      { // Turn off
        If(LOr(LEqual(And(SDS1,0x0800), 0x0800),LEqual(And(SDS1,0x1000), 0x1000))){    //Precision Enabled
          //mask interrupt from Touch Screen GPIPIRQ2IOXAPIC: GPI PIRQ[X:I] to IOxAPIC[39:24] Enable
          \_SB.GP2A(50, Zero) //TPL0
          \_SB.WTGP(89,0)
          \_SB.WTGP(52,0) //GPIO RST to Touch Panel
          Store(Zero , ^ONTM)
        }
        Else{
          //mask interrupt from Touch Screen and Touch Pad GPIPIRQ2IOXAPIC: GPI PIRQ[X:I] to IOxAPIC[39:24] Enable
          \_SB.GP2A(50, Zero) //TPL0
          \_SB.GP2A(55, Zero) //TPD0/1
          \_SB.WTGP(88,0)
          \_SB.WTGP(89,0)
          \_SB.WTGP(52,0) //GPIO RST to Touch Panel
          Store(Zero , ^ONTM)
        }
      }
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

    Scope(TPD0){
      Method(_PR0, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x04), 0x04)) 
        {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x0004), 0x0004)) 
        {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }      
    }

    Scope(TPL1){
      Method(_PS0,0,Serialized)
      {
        If(LEqual(\_SB.PCI0.I2C1.PXTC.ONTM,Zero)) {
          Return()
        }
        // Make sure "D0 delay" (IC1D) delay is elapsed before returning _PS0
        Divide(Subtract(Timer(), \_SB.PCI0.I2C1.PXTC.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
        If(LLess(Local0, IC1D)) //Do not sleep if already past the delay requirement 100ms for Touchpanel
        {
          // Delay for power ramp
          Sleep(Subtract(IC1D, Local0)) //Sleep (IC1D - time elapsed)
        }
      }

      Method(_PS3,0,Serialized)
      {
      }

      Method(_PR0, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x0002), 0x0002))
        {
          Return(Package(){\_SB.PCI0.I2C0.SHUB.PSEN,PXTC})
        }Else{
         Return(Package(){})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {      
        If(LEqual(And(SDS1,0x0002), 0x0002))
        {
          Return(Package(){\_SB.PCI0.I2C0.SHUB.PSEN,PXTC})
        }Else{
         Return(Package(){})
        }
      }
    }

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
          Return(Package(){PXTC})
        } Else {
          Return(Package() {})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x08), 0x08)) {
          Return(Package(){PXTC})
        } Else {
          Return(Package() {})
        }
      }
    }// End of Scope(TPD1)
    
    //------------------------------
    //  NTRIG Digitizer Touch Panel
    //------------------------------
    Scope(TPL2){
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
        If(LEqual(And(SDS1,0x0020), 0x0020)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x0020), 0x0020)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
    }// End of Scope(TPL2)

    //------------------------
    //  EETI Touch Panel
    //------------------------   
    Scope(TPL3){
      Method(_PS0,0,Serialized)
      {
        If(LEqual(\_SB.PCI0.I2C1.PXTC.ONTM,Zero)) {
          Return()
        }
        // Make sure "D0 delay" (IC1D) delay is elapsed before returning _PS0
        Divide(Subtract(Timer(), \_SB.PCI0.I2C1.PXTC.ONTM), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
        If(LLess(Local0, IC1D)) //Do not sleep if already past the delay requirement 100ms for Touchpanel
        {
          // Delay for power ramp
          Sleep(Subtract(IC1D, Local0)) //Sleep (IC1D - time elapsed)
        }
      }

      Method(_PS3,0,Serialized)
      {
      }

      Method(_PR0, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x0040), 0x0040)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x0040), 0x0040)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
    }// End of Scope(TPL3)
    

    //------------------------
    //  Alps touchpad
    //------------------------
    Scope(TPD2){
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
        If(LEqual(And(SDS1,0x0080), 0x0080)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x0080), 0x0080)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
    }// End of Scope(TPD2)

    //------------------------
    //  Cypress touchpad
    //------------------------
    Scope(TPD3){
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
        If(LEqual(And(SDS1,0x0100), 0x0100)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj)
      {
        If(LEqual(And(SDS1,0x0100), 0x0100)) {
          Return(Package(){PXTC})
        }Else{
         Return(Package(){})
        }
      }
    }// End of Scope(TPD3)

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
  //GPE Event handling - Start
  Scope(\_GPE) {
  
  Method(_L11) { // Zpodd GPIO handler
    Store(1,\GO17)     // Clear GPI ownership to stop SCI flooding
    If(LEqual(\_SB.PCI0.SAT0.PRT1.UDSM, Zero)) {
      Notify(\_SB.PCI0.SAT0.PRT1, 2)  // Device Wake to Device (Windows)
    }Else{
      Notify(\_SB.PCI0.SAT0, 0x81) //Notify Port 1 to wake from DSM method
    } // \_SB.PCI0.SAT0.UDSM
  }
    //
    // GPIO8 PCIE slot 3 wake event
    //
    Method(_L08) {
      // PCIe WAKE#
      If(\_SB.PCI0.RP03.WKEN){
        //GPIO_OWN to GPIO Driver to stop SCI flooding
        Store(1, \GO08)
        Notify(\_SB.PCI0.RP03, 0x02)  // DeviceWake
      }
    }
    //
    // GPIO45(0x2D) PCIE slot 4 wake event
    //
    Method(_L2D) {
      // PCIe WAKE#
      If(\_SB.PCI0.RP04.WKEN){
        //GPIO_OWN to GPIO Driver to stop SCI flooding
        Store(1, \GO45)
        Notify(\_SB.PCI0.RP04, 0x02)  // DeviceWake
      }
    }

    //
    // GPIO51(0x33) PCIE slot 5 wake event
    //
    Method(_L33) {
      // PCIe WAKE#
      If(\_SB.PCI0.RP05.WKEN){
        //GPIO_OWN to GPIO Driver to stop SCI flooding
        Store(1, \GO51)
        Notify(\_SB.PCI0.RP05, 0x02)  // DeviceWake
      }
    }
  } //Scope(\_GPE)

If(LEqual(RWAG,0)) {// standard board configuration, no W/A bue wires for slot 1 or 6
    //SDIO Reset support
    Scope(\_SB.PCI0.SDHC) {
      // D0 Method for SDHC
        Method(PS0X,0,Serialized)
        {
          // Drive GPIO77 High - 1: No GPIO RST to on board mPCIe/NGFF_WIFI
          \_SB.WTGP(77,1)
        }
    }

  Scope(\_SB.PCI0.SDHC.WI01) {
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
}

  Scope(\_SB.PCI0.XHC){ //USB XHCI 
  
  Name(UPWR,0)
  Name(USPP,0)
  // Bit 0 : USB SD Card Reader
  // Bit 1 : USB(HS1 : 0x480 /SS0 : 0x510)
  // BIT 2 : USB(HS2 : 0x490 / SS1 : 0x520)

  
  Method(DUAM,0,Serialized)
  {

    OperationRegion (XHCM, SystemMemory, And(MEMB,0xFFFF0000), 0x600)
    Field (XHCM, DWordAcc, NoLock, Preserve)
    {
      Offset(0x2),
      XHCV,16,
      Offset(0x480),
      HP01, 1, // HS port 1
      Offset(0x490),
      HP02, 1, // HS port 2
      Offset(0x510),
      SP00, 1, // SS port 0
      Offset(0x520),
      SP01, 1, // SS port 1
    }

  if(LNotEqual(\UAMS,0x00)) // User not present
  {
    Store(0x00,UPWR)
    // Enterng CS, Remove power if SD card not present

    If(LEqual(XHCV,0xFFFF)) // Controller in D3Hot(MEM_BASE not decoded)
    {
      If(And(USPP,0x02)) // USB(HS1 : 0x480 /SS0 : 0x510)
      {
        \_SB.WTGP(13,0)
        Or(UPWR,0x02, UPWR)
      }
      If(And(USPP,0x04)) // USB(HS2 : 0x490 / SS1 : 0x520)
      {
        \_SB.WTGP(43,0)
        Or(UPWR,0x04, UPWR)
      }
    }Else{ //    If(Lequal(XHCV,0xFFFF))
        If(LAnd(LEqual(HP01,0x00),LEqual(SP00,0x00))) // USB(HS1 : 0x480 /SS0 : 0x510)
        {
          \_SB.WTGP(13,0)
          Or(UPWR,0x02, UPWR)
        }
        If(LAnd(LEqual(HP02,0x00),LEqual(SP01,0x00))) // USB(HS2 : 0x490 / SS1 : 0x520)
        {
          \_SB.WTGP(43,0)
          Or(UPWR,0x04, UPWR)
        }
    }//    If(Lequal(XHCV,0xFFFF))
  }Else{ //if(LNotEqual(\UAMS,0x00))

    // Exiting CS , Apply power if power removed
    if(And(UPWR,0x02)) // USB(HS1 : 0x480 /SS0 : 0x510)
    {
      \_SB.WTGP(13,1)
    }
    if(And(UPWR,0x04)) // USB(HS2 : 0x490 / SS1 : 0x520)
    {
      \_SB.WTGP(43,1)
    }
  } //if(LNotEqual(\UAMS,0x00)) // User not present
  } // End of DUAM
  }
} //If(LAnd(LEqual(BID,BWT1),LEqual(RTD3,1)))
} // End SSDT

