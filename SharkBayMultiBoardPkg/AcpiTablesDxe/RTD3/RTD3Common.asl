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

External(\_SB.OSCO)
External(\_SB.PCI0,DeviceObj)
External(\_SB.PCI0.SAT0, DeviceObj)
External(\_SB.PCI0.SAT0.PRT0, DeviceObj)
External(\_SB.PCI0.SAT0.PRT1, DeviceObj)
External(\_SB.PCI0.SAT0.PRT2, DeviceObj)

External(\_SB.PCI0.RP01, DeviceObj)
External(\_SB.PCI0.RP03, DeviceObj)
External(\_SB.PCI0.RP04, DeviceObj)
External(\_SB.PCI0.RP05, DeviceObj)
External(\_SB.PCI0.RP06, DeviceObj)
External(\_SB.PCI0.RP01.LDIS)
External(\_SB.PCI0.RP03.LDIS)
External(\_SB.PCI0.RP04.LDIS)
External(\_SB.PCI0.RP05.LDIS)
External(\_SB.PCI0.RP06.LDIS)
External(\_SB.PCI0.RP01.L23E)
External(\_SB.PCI0.RP03.L23E)
External(\_SB.PCI0.RP04.L23E)
External(\_SB.PCI0.RP05.L23E)
External(\_SB.PCI0.RP06.L23E)
External(\_SB.PCI0.RP01.L23R)
External(\_SB.PCI0.RP03.L23R)
External(\_SB.PCI0.RP04.L23R)
External(\_SB.PCI0.RP05.L23R)
External(\_SB.PCI0.RP06.L23R)
External(\_SB.PCI0.RP01.LEDM)
External(\_SB.PCI0.RP03.LEDM)
External(\_SB.PCI0.RP04.LEDM)
External(\_SB.PCI0.RP05.LEDM)
External(\_SB.PCI0.RP06.LEDM)
External(\_SB.PCI0.RP01.LASX)
External(\_SB.PCI0.RP03.LASX)
External(\_SB.PCI0.RP04.LASX)
External(\_SB.PCI0.RP05.LASX)
External(\_SB.PCI0.RP06.LASX)
External(\CKEN)
External(\PMFS)

External(\_SB.PCI0.XHC, DeviceObj)
External(\_SB.PCI0.XHC.PMES)
External(\_SB.PCI0.XHC.PMEE)
External(\_SB.PCI0.XHC.MEMB)
External(\_SB.PCI0.EHC2, DeviceObj)
External(\_SB.PCI0.EHC2.PMES)
External(\_SB.PCI0.EHC2.PMEE)
External(\_SB.PCI0.XHC.RHUB, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.SSP1, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.SSP2, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.HS01, DeviceObj) //xHCI HSP port 1
External(\_SB.PCI0.XHC.RHUB.HS02, DeviceObj) //xHCI HSP port 2
External(\_SB.PCI0.I2C0, DeviceObj) //I2C Controller
External(\_SB.PCI0.I2C0.SHUB, DeviceObj) // Sensor hub

External(PEP0)
External(PEP3)
External(XHPR)
External(RCG0, IntObj)     // RTD3 Config Setting(BIT0:ZPODD,BIT1:USB Camera Port4, BIT2/3:SATA Port3)
External(\GPRW, MethodObj)
External(P8XH, MethodObj)
External(BID, IntObj)         // BoardId
External(RTD3, IntObj)
External(XDST, IntObj)

//
// Externs common to ULT0RTD3.asl and FFRDRTD3.asl and exclude for BRRTD3.asl
//
#ifndef BRRTD3 
External(\_SB.GP2A, MethodObj)
External(\_SB.GP2B, MethodObj)
External(\_SB.GP2N, MethodObj)
External(\_SB.RDGP, MethodObj)
External(\_SB.WTGP, MethodObj)
External(\_SB.WTIN, MethodObj)
External(\_SB.RDGI, MethodObj)

External(\_SB.PCI0.HDEF, DeviceObj)
External(\_SB.PCI0.ADSP, DeviceObj)
External(\_SB.PCI0.I2C1, DeviceObj) //I2C1 Controller
External(\_SB.PCI0.I2C1.TPD1, DeviceObj) // Touch pad
External(\_SB.PCI0.I2C1.TPL0, DeviceObj) // Touch panel
External(\_SB.PCI0.I2C1.TPD0, DeviceObj) // Touch pad
External(\_SB.PCI0.LPCB.H_EC.SPT2)            // SATA_CABLE_PWR_EN bit in EC
External(\_SB.PCI0.LPCB.H_EC.ECMD, MethodObj) // EC Command method
External(\_SB.PCI0.LPCB.H_EC.ECAV, IntObj)    // EC Driver loaded flag
External(\_SB.PCI0.LPCB.H_EC.ECRD, MethodObj) // EC Read Method
External(\_SB.PCI0.LPCB.H_EC.ECWT, MethodObj) // EC Write Method  
External(\_SB.PCI0.SAT0.PRT3, DeviceObj)

External(\UAMS)
External(RIC0,FieldUnitObj)
External(SDS0,FieldUnitObj)
External(SDS1,FieldUnitObj)
External(IC1D,FieldUnitObj)
External(IC1S,FieldUnitObj)
External(IC0D,FieldUnitObj)
External(I20D,FieldUnitObj)
External(I21D,FieldUnitObj)
External(AUDD,FieldUnitObj)
External(HDAD,FieldUnitObj)
External(SHSB,FieldUnitObj)
External(VRSD,FieldUnitObj)
External(VRRD,FieldUnitObj)

External(\GO08)
External(\GO45)
External(\GO51)
External(\GS08)
External(\GS45)
External(\GS51)

Name(LONT, 0)           // Last ON Timestamp: The time stamp of the last RTDpower resource _ON method evaluation 

//
// SGON - Staggering ON Method with VR Staggering delay
//    Staggering ON Method with VR Staggering (VRSD) delay.  Sleep the extra remaining amount of time if necessary.
//    Update the global running timer of LONT (Last ON mehtod timestamp)
//  
//    Arguments:
//      Arg0 - GPIO Pin number to toggle power on
//      Arg1 - GPIO Value, 0: Active Low, 1: Active High
//
//    Return Value:
//      Zero - VR staggering is skipped
//      One  - VR staggering is performed
//
Method(SGON, 2, Serialized)
{
  // Check if device is already driven to power on
  If(LNotEqual(\_SB.RDGP(Arg0), Arg1)) {
    // Check for VR staggering 
    Divide(Subtract(Timer(), \LONT), 10000, , Local0) //Store Elapsed time in ms, ignore remainder
    If(LLess(Local0, \VRSD)) { //Do not sleep if already past the delay requirement
      // Delay for power ramp using Timer Based Sleep
      Sleep(Subtract(\VRSD, Local0))
    }

    \_SB.WTGP(Arg0, Arg1) // Drive GPIO to power on device

    Store(Timer(), \LONT) // Update the global Last ON Method Timer

    Return(One)  // VR staggering is performed
  } Else {
    Return(Zero) // VR staggering is skipped
  }
}

#endif

