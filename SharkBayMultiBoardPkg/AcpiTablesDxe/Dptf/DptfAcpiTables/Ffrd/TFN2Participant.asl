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

  TFN2Participant.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/

Scope(\_SB.PCI0.LPCB.H_EC)
{

  Device(TFN2)
  {
    Name(_HID, EISAID("INT3404"))  // Intel DPTF Fan Device
    Name(_UID, 1)
    Name(_STR, Unicode ("CPU Fan #2"))
    
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
      If (LEqual(FND2,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // _FIF (Fan Information)
    //
    // The optional _FIF object provides OSPM with fan device capability information.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //  A Package containing the fan device parameters.
    //
    Method(_FIF)
    {
        Return (Package (){
        0,                            // Revision:Integer
        1,                            // FineGrainControl:Integer Boolean
        2,                            // StepSize:Integer DWORD
        0                             // LowSpeedNotificationSupport:Integer Boolean
        })
    }

    // _FPS (Fan Performance States)
    //
    // Evaluates to a variable-length package containing a list of packages that describe the fan device's performance states.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //  A variable-length Package containing a Revision ID and a list of Packages that describe the fan device's performance states.
    //
    Method(_FPS,,,,PkgObj)
    {
        Return (Package()
        {
          0,    // Revision:Integer
          //        Control,  TripPoint,    Speed,  NoiseLevel, Power
          Package(){100,      0xFFFFFFFF,   7000,   500,        5000},
          Package(){ 95,      0xFFFFFFFF,   6650,   475,        4750},
          Package(){ 90,      0xFFFFFFFF,   6300,   450,        4500},
          Package(){ 85,      0xFFFFFFFF,   5950,   425,        4250},
          Package(){ 80,      0xFFFFFFFF,   5600,   400,        4000},
          Package(){ 75,      0xFFFFFFFF,   5250,   375,        3750},
          Package(){ 70,      0xFFFFFFFF,   4900,   350,        3500},
          Package(){ 60,      0xFFFFFFFF,   4200,   300,        3000},
          Package(){ 50,      0xFFFFFFFF,   3500,   250,        2500},
          Package(){ 40,      0xFFFFFFFF,   2800,   200,        2000},
          Package(){ 25,      0xFFFFFFFF,   1750,   125,        1250},
          Package(){  0,      0xFFFFFFFF,   0,      0,          0}   // OFF
        })
    }

    // _FSL (Fan Set Level)
    //
    // The optional _FSL object is a control method that OSPM evaluates to set a fan device's speed (performance state) to a specific level.
    //
    // Arguments: (1)
    //  Arg0 - Level (Integer): conveys to the platform the fan speed level to be set.
    // Return Value:
    //  None
    //
    // Argument Information
    //  Arg0: Level. If the fan supports fine-grained control, Level is a percentage of maximum level (0-100) 
    //  that the platform is to engage the fan. If the fan does not support fine-grained control, 
    //  Level is a Control field value from a package in the _FPS object's package list. 
    //  A Level value of zero causes the platform to turn off the fan.
    // 
    Method(_FSL,1,Serialized)
    {
      If(\ECON)
      {
        \_SB.PCI0.LPCB.H_EC.ECWT(2, RefOf(\_SB.PCI0.LPCB.H_EC.PPSL))
        \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PPSH))
        \_SB.PCI0.LPCB.H_EC.ECWT(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PENV)), RefOf(\_SB.PCI0.LPCB.H_EC.PINV))
        \_SB.PCI0.LPCB.H_EC.ECWT(Arg0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV))
        \_SB.PCI0.LPCB.H_EC.ECWT(100, RefOf(\_SB.PCI0.LPCB.H_EC.PSTP))
        \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)      // Command to turn the fan on
        Notify(\_SB.IETM, 0x83) // Notify DPTF TZ that fan speed has changed
      }
    }

    Name(TFST, Package()
    {
        0,          // Revision:Integer
        0xFFFFFFFF, // Control:Integer DWORD
        0xFFFFFFFF  // Speed:Integer DWORD
    })

    //_FST (Fan Status)
    //
    // The optional _FST object provides status information for the fan device.
    //
    // Arguments: (0)
    //  None
    // Return Value:
    //  A Package containing fan device status information
    //
    Method(_FST,,,,PkgObj)
    {
      Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PENV)), Index(TFST, 1))
      Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CFS2)), Index(TFST, 2))
      Return(TFST)
    }

  } // End TFN2 Device
}// end Scope(\_SB.PCI0.LPCB.H_EC)
