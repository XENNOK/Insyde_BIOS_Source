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

  Dptf.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/

#include "PowerMgmtDefinitions.h"

DefinitionBlock (
	"Dptf.aml",
	"SSDT",
	1,
	"DptfTabl",
	"DptfTabl",
	0x1000
    )
{


External(P8XH, MethodObj)
External(ECON, IntObj)
External(TSOD, IntObj)
External(PNHM, IntObj)
External(TCNT, IntObj)
External(\MPLT, IntObj)
External(\PWRS, Intobj)
External(LPMV, IntObj)
External(CRTT, IntObj)
External(ATMC, IntObj)
External(PTMC, IntObj)
External(ATRA, IntObj)
External(PTRA, IntObj)
External(ACTT, IntObj)
External(PSVT, IntObj)
External(ATPC, IntObj)
External(PTPC, IntObj)

External(DPTF, IntObj)  // EnableDptf
External(SADE, IntObj)  // EnableSaDevice
External(SACR, IntObj)  // CriticalThermalTripPointSa
External(SAHT, IntObj)  // HotThermalTripPointSa

External(PCHD, IntObj)  // EnablePchDevice
External(PCHC, IntObj)  // CriticalThermalTripPointPch
External(PCHH, IntObj)  // HotThermalTripPointPch

External(MEMD, IntObj)  // EnableMemoryDevice
External(MEMC, IntObj)  // CriticalThermalTripPointTMEM
External(MEMH, IntObj)  // HotThermalTripPointTMEM

External(FND1, IntObj)  // EnableFan1Device
External(FND2, IntObj)  // EnableFan2Device

External(AMBD, IntObj)  // EnableAmbientDevice
External(AMAT, IntObj)  // ActiveThermalTripPointAmbient
External(AMPT, IntObj)  // PassiveThermalTripPointAmbient
External(AMCT, IntObj)  // CriticalThermalTripPointAmbient
External(AMHT, IntObj)  // HotThermalTripPointAmbient

External(SKDE, IntObj)  // EnableSkinDevice
External(SKAT, IntObj)  // ActiveThermalTripPointSkin
External(SKPT, IntObj)  // PassiveThermalTripPointSkin
External(SKCT, IntObj)  // CriticalThermalTripPointSkin
External(SKHT, IntObj)  // HotThermalTripPointSkin

External(EFDE, IntObj)  // EnableExhaustFanDevice
External(EFAT, IntObj)  // ActiveThermalTripPointExhaustFan
External(EFPT, IntObj)  // PassiveThermalTripPointExhaustFan
External(EFCT, IntObj)  // CriticalThermalTripPointExhaustFan
External(EFHT, IntObj)  // HotThermalTripPointExhaustFan

External(VRDE, IntObj)  // EnableVRDevice
External(VRAT, IntObj)  // ActiveThermalTripPointVR
External(VRPT, IntObj)  // PassiveThermalTripPointVR
External(VRCT, IntObj)  // CriticalThermalTripPointVR
External(VRHT, IntObj)  // HotThermalTripPointVR

External(WIFD, IntObj)  // EnableWifiDevice
External(WFAT, IntObj)  // ActiveThermalTripPointWifi
External(WFPT, IntObj)  // PassiveThermalTripPointWifi
External(WFCT, IntObj)  // CriticalThermalTripPointWifi
External(WFHT, IntObj)  // ActiveHotThermalTripPointWifi

External(WAND, IntObj)  // EnableWWANDevice
External(WWAT, IntObj)  // ActiveThermalTripPointWWAN
External(WWPT, IntObj)  // PassiveThermalTripPointWWAN
External(WWCT, IntObj)  // CriticalThermalTripPointWWAN
External(WWHT, IntObj)  // ActiveHotThermalTripPointWWAN

External(WGID, IntObj)  // EnableWGigDevice
External(WGAT, IntObj)  // ActiveThermalTripPointWGig
External(WGPT, IntObj)  // PassiveThermalTripPointWGig
External(WGCT, IntObj)  // CriticalThermalTripPointWGig
External(WGHT, IntObj)  // ActiveHotThermalTripPointWGig

External(CTDP, IntObj)  // EnableCtdpPolicy
External(LPMP, IntObj)  // EnableLpmPolicy
External(DPAP, IntObj)  // EnableActivePolicy
External(DPPP, IntObj)  // EnablePassivePolicy
External(DPCP, IntObj)  // EnableCriticalPolicy
External(DCMP, IntObj)  // EnableCoolingModePolicy
External(ECEU, IntObj)  // EnableCurrentExecutionUnit
External(TGFG, IntObj)  // TargetGfxFreq
External(TRTV, IntObj)  // TrtRevision
External(PWRE, IntObj)  // EnablePowerDevice
External(PWRP, IntObj)  // EnablePowerPolicy

External(LPOE, IntObj)  // LPOEnable
External(LPOP, IntObj)  // LPOStartPState
External(LPOS, IntObj)  // LPOStepSize
External(LPOW, IntObj)  // LPOPowerControlSetting
External(LPER, IntObj)  // LPOPerformanceControlSetting

External(PPSZ, IntObj)  // PPCC Step Size
External(\PDC0, IntObj) // CPU0 _PDC Flags

External(\_TZ.ETMD, IntObj)
External(\_TZ.TZ00, ThermalZoneObj)
External(\_TZ.TZ01, ThermalZoneObj)

External(\_SB.PCI0, DeviceObj)
External(\_SB.PCI0.B0D4, DeviceObj)
External(\_SB.PCI0.MHBR, FieldUnitObj)

External(\_SB.PCI0.LPCB.H_EC, DeviceObj)
External(\_SB.PCI0.LPCB.H_EC.ECAV, IntObj)
External(\_SB.PCI0.LPCB.H_EC.ECMD, MethodObj)
External(\_SB.PCI0.LPCB.H_EC.ECRD, MethodObj)    // EC Read Method
External(\_SB.PCI0.LPCB.H_EC.ECWT, MethodObj)    // EC Write Method 
External(\_SB.PCI0.LPCB.H_EC.ECF2, OpRegionObj)
External(\_SB.PCI0.LPCB.H_EC.SKNT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CVRT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.FANT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.AMBT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PPSL, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PPSH, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PINV, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PENV, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PSTP, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CMDR, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSD0, FieldUnitObj) 
External(\_SB.PCI0.LPCB.H_EC.TSD1, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.DIM0, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.DIM1, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CFSP, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CPUP, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CPAP, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PMAX, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PECH, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CFAN, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSI, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.HYST, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSHT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSLT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSSR, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.B1RC, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.B1FC, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.BAT1._BST, MethodObj)
External(\_SB.PCI0.LPCB.H_EC.B1ML, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.B1MH, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PPWR, FieldUnitObj)

External(\_PR.CPU0, ProcessorObj)
External(\_PR.CPU0._PSS, MethodObj)
External(\_PR.CPU0._PPC, IntObj)
External(\_PR.CPU0._TSS, MethodObj)
External(\_PR.CFGD, FieldUnitObj)
External(\_PR.CPU0.TSMF, PkgObj)
External(\_PR.CPU0.TSMC, PkgObj)
External(\_PR.CPU0._PTC, MethodObj)
External(\_PR.CPU0._TSD, MethodObj)
External(\_PR.CPU0._TPC, IntObj)
External(\_PR.CPU1, ProcessorObj)
External(\_PR.CPU2, ProcessorObj)
External(\_PR.CPU3, ProcessorObj)
External(\_PR.CPU4, ProcessorObj)
External(\_PR.CPU5, ProcessorObj)
External(\_PR.CPU6, ProcessorObj)
External(\_PR.CPU7, ProcessorObj)

External(\_PR.CLVL, FieldUnitObj)
External(\_PR.CBMI, FieldUnitObj)
External(\_PR.PL10, FieldUnitObj)
External(\_PR.PL20, FieldUnitObj)
External(\_PR.PLW0, FieldUnitObj)
External(\_PR.CTC0, FieldUnitObj)
External(\_PR.TAR0, FieldUnitObj)
External(\_PR.PL11, FieldUnitObj)
External(\_PR.PL21, FieldUnitObj)
External(\_PR.PLW1, FieldUnitObj)
External(\_PR.CTC1, FieldUnitObj)
External(\_PR.TAR1, FieldUnitObj)
External(\_PR.PL12, FieldUnitObj)
External(\_PR.PL22, FieldUnitObj)
External(\_PR.PLW2, FieldUnitObj)
External(\_PR.CTC2, FieldUnitObj)
External(\_PR.TAR2, FieldUnitObj)

External(\_PR.APSV, FieldUnitObj)
External(\_PR.AAC0, FieldUnitObj)
External(\_PR.ACRT, FieldUnitObj)

External(\_SB.PCCD.PENB, IntObj)

// Display participant externals
External(DISE, IntObj)  // EnableDisplayParticipant
External(\_SB.PCI0.GFX0.DD1F._BCL, MethodObj)
External(\_SB.PCI0.GFX0.DD1F._BCM, MethodObj)
External(\_SB.PCI0.GFX0.DD1F._BQC, MethodObj)
External(\_SB.PCI0.GFX0.DD1F._DCS, MethodObj)

Scope(\_SB)
{
  //
  // DPTF Thermal Zone Device
  // 
  //
  Device(IETM)
  {
    //
    // Intel DPTF Thermal Framework Device
    //
    Name(_HID, EISAID("INT3400"))

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
      If (LEqual(DPTF,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    //
    // Note: there are seven GUID packages in TMPP and up to seven matching store statements in IDSP.  
    // The store statements must never exceed the number of package elements to prevent an overrun.
    //
    Name(TMPP,Package()
    {
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000")
    })

    //
    // Save original trip points so _OSC method can enable/disable Legacy thermal policies by manipulating trip points.
    //
    Name (PTRP,0)  // Passive trip point
    Name (PSEM,0)  // Passive semaphore
    Name (ATRP,0)  // Active trip point
    Name (ASEM,0)  // Active semaphore
    Name (YTRP,0)  // Critical trip point
    Name (YSEM,0)  // Critical semaphore

    // IDSP (Intel DPTF Supported Policies)
    //
    // This object evaluates to a package of packages, with each package containing the UUID
    // values to represent a policy implemented and supported by the Intel DPTF software stack.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Package of Guid packages
    //
    Method(IDSP,0,Serialized,,PkgObj)
    {
        Name(TMPI,0)

        // Passive Policy GUID
        If(LAnd(LEqual(DPPP,1),CondRefOf(DPSP))){
          Store(DeRefOf(Index(DPSP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Active Policy GUID
        If(LAnd(LEqual(DPAP,1),CondRefOf(DASP))){
          Store(DeRefOf(Index(DASP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Critical Policy GUID
        If(LAnd(LEqual(DPCP,1),CondRefOf(DCSP))){
          Store(DeRefOf(Index(DCSP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Cooling Mode Policy GUID
        If(LAnd(LEqual(DCMP,1),CondRefOf(DMSP))){
          Store(DeRefOf(Index(DMSP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // LPM policy GUID
        If(LAnd(LEqual(LPMP,1),CondRefOf(LPSP))){
          Store(DeRefOf(Index(LPSP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // CTDP Policy GUID
        If(LAnd(LEqual(CTDP,1),CondRefOf(CTSP))){
          Store(DeRefOf(Index(CTSP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        //Power Policy GUID
        If(LAnd(LEqual(PWRP,1),CondRefOf(WPSP))){
          Store(DeRefOf(Index(WPSP,0)), Index(TMPP,TMPI))
        }

        Return(TMPP)
    }

    // _OSC (Operating System Capabilities)
    //
    // This object is evaluated by each DPTF policy implementation to communicate to the platform of the existence and/or control transfer.
    //
    // Arguments: (4)
    //   Arg0 - A Buffer containing a UUID
    //   Arg1 - An Integer containing a Revision ID of the buffer format
    //   Arg2 - An Integer containing a count of entries in Arg3
    //   Arg3 - A Buffer containing a list of DWORD capabilities
    // Return Value:
    //   A Buffer containing a list of capabilities
    //
    Method(_OSC, 4,Serialized,,BuffObj,{BuffObj,IntObj,IntObj,BuffObj})
    {      
      Name(NUMP,0)
      Name (UID2,ToUUID("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"))

      // Point to Status DWORD in the Arg3 buffer (STATUS)
      CreateDWordField(Arg3, 0, STS1)

      // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
      CreateDWordField(Arg3, 4, CAP1)

      //
      // _OSC needs to validate the UUID and Revision.
      //
      // IF Unrecognized UUID
      //  Return Unrecognized UUID _OSC Failure
      // IF Unsupported Revision
      //  Return Unsupported Revision _OSC Failure
      //
      //    STS0[0] = Reserved
      //    STS0[1] = _OSC Failure
      //    STS0[2] = Unrecognized UUID
      //    STS0[3] = Unsupported Revision  
      //    STS0[4] = Capabilities masked
      //
      // Get the platform UUID's that are available, this will be a package of packages.
      //
      IDSP()                     // initialize TMPP with GUID's
      Store(SizeOf(TMPP),NUMP)   // how many GUID's in the package?

      // Note:  The comparison method used is necessary due to
      // limitations of certain OSes which cannot perform direct
      // buffer comparisons.
      //
      // Create a set of "Input" UUID fields.
      //
      CreateDWordField(Arg0, 0x0, IID0)
      CreateDWordField(Arg0, 0x4, IID1)
      CreateDWordField(Arg0, 0x8, IID2)
      CreateDWordField(Arg0, 0xC, IID3)
      //
      // Create a set of "Expected" UUID fields.
      //
      CreateDWordField(UID2, 0x0, EID0)
      CreateDWordField(UID2, 0x4, EID1)
      CreateDWordField(UID2, 0x8, EID2)
      CreateDWordField(UID2, 0xC, EID3)
      //
      // Compare the input UUID to the list of UUID's in the system.
      //
      While(NUMP){
        //
        // copy one uuid from TMPP to UID2
        //
        Store(DeRefOf (Index (TMPP, Subtract(NUMP,1))),UID2)
        //
        // Verify the input UUID matches the expected UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          Break  // break out of while loop when matching UUID is found
        }
        Decrement(NUMP)
      }

      If(LEqual(NUMP,0)){
        //
        // Return Unrecognized UUID _OSC Failure
        //
        And(STS1,0xFFFFFF00,STS1)
        Or(STS1,0x6,STS1)
        Return(Arg3)
      }
      
      If(LNot(LEqual(Arg1, 1))) 
      {
        //
        // Return Unsupported Revision _OSC Failure
        //
        And(STS1,0xFFFFFF00,STS1)
        Or(STS1,0xA,STS1)
        Return(Arg3)
      }
         
      If(LNot(LEqual(Arg2, 2)))
      {
        //
        // Return Argument 3 Buffer Count not sufficient
        //
        And(STS1,0xFFFFFF00,STS1)
        Or(STS1,0x2,STS1)
        Return(Arg3)
      }        

      //
      // Passive Policy GUID
      //
      If(LAnd(LEqual(DPPP,1),CondRefOf(\_PR.APSV))){
        If(LEqual(PSEM,0)){
          Store(1,PSEM)
          Store(\_PR.APSV,PTRP)  // use semaphore so variable is only initialized once
        }
        //
        // copy the GUID to UID2
        //
        If(CondRefOf(DPSP)){
          Store(DeRefOf (Index (DPSP, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          // do passive notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x01))
            { // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(110,\_PR.APSV)
            } Else {  // Passive unloading, re-enable legacy thermal zone
              Store(PTRP,\_PR.APSV)
            }
          // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
          Notify(\_TZ.TZ00, 0x80)
          Notify(\_TZ.TZ01, 0x80)
          }
          Return(Arg3)
        }
      }

      //
      // Active Policy GUID
      //
      If(LAnd(LEqual(DPAP,1),CondRefOf(\_PR.AAC0))){
        If(LEqual(ASEM,0)){
          Store(1,ASEM)
          Store(\_PR.AAC0,ATRP)  // use semaphore so variable is only initialized once
        }
        //
        // copy the GUID to UID2
        //
        If(CondRefOf(DASP)){
          Store(DeRefOf (Index (DASP, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          // do active notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x01))
            { // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(110,\_PR.AAC0)
            } Else {  // Passive unloading, re-enable legacy thermal zone
              Store(ATRP,\_PR.AAC0)
            }
          // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
          Notify(\_TZ.TZ00, 0x80)
          Notify(\_TZ.TZ01, 0x80)
          }
          Return(Arg3)
        }
      }

      //
      // Critical Policy GUID
      //
      If(LAnd(LEqual(DPCP,1),CondRefOf(\_PR.ACRT))){
        If(LEqual(YSEM,0)){
          Store(1,YSEM)
          Store(\_PR.ACRT,YTRP)  // use semaphore so variable is only initialized once
        }
        //
        // copy the GUID to UID2
        //
        If(CondRefOf(DCSP)){
          Store(DeRefOf (Index (DCSP, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          // do critical notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x01))
            { // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(210,\_PR.ACRT)
            } Else {  // Passive unloading, re-enable legacy thermal zone
              Store(YTRP,\_PR.ACRT)
            }
          // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
          Notify(\_TZ.TZ00, 0x80)
          Notify(\_TZ.TZ01, 0x80)
          }
          Return(Arg3)
        }
      }

      Return(Arg3)
    }
    // KTOC (Kelvin to Celsius)
    //
    // This control method converts from 10ths of degree Kelvin to Celsius.
    //
    // Arguments: (1)
    //   Arg0 - Temperature in 10ths of degree Kelvin
    // Return Value:
    //   Temperature in Celsius
    //
    Method(KTOC,1)
    {
      Subtract(Arg0, 2732, Local1)
      If(LGreater(Local1,0)) // make sure we don't have a negative number
      {
        Return(Divide(Local1, 10))
      }
      Else
      {
        Return(0)
      }
    }
   
    // CTOK (Celsius to Kelvin)
    //
    // This control method converts from Celsius to 10ths of degree Kelvin.
    //
    // Arguments: (1)
    //   Arg0 - Temperature in Celsius
    // Return Value:
    //   Temperature in 10ths of degree Kelvin
    //
    Method(CTOK, 1)
    {
      If(And(Arg0, 0x1000)) // Round negative values up to Zero
      {
        Return(2372)
      }
      Multiply(Arg0,10,Local0)
      And(Local0, 0x0FFF, Local1)
      Add(Local1, 2732, Local2)      
      Return(Local2)
    }

    // PDRT (power device relation table)
    //
    // This object evaluates to a package of packages that indicates the relation between charge rates and target devices.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length package of packages as described below.
    //
    // Return Value Information
    //   Package {
    //   Package [0] // Package
    //   ....
    //   Package [n] // Package
    //   }
    //
    //   Each sub-Package contains the elements described below:
    //   Package {
    //     ChargeRate    // Integer (DWORD)
    //     TargetDevice  // Integer (DWORD)
    //     PTYP          // Integer (DWORD)
    //     ControlBuffer // Package
    //   }
    //
    Method(PDRT,,,,PkgObj)
    { 
      \_SB.PCI0.B0D4.PCCC() // init global variable PCCX
      If (\PWRS){ // check power source
        PDAC() // AC table
      } Else {
        PDDC() // DC table
      }
    } // end Method(PDRT)

    Method(PDDC,,,,PkgObj)
    { 
      Name(TMPD,Package()
      { 
        Package() {80, \_SB.PCI0.B0D4, 0x09, Package () {0x00010001,0x80000000}}, // Package domain, PL2, target power from CPU PPM
        Package() {60, \_SB.PCI0.B0D4, 0x09, Package () {0x00010000,0x80000000}}, // Package domain, PL1, target power from CPU PPM
        Package() {55, \_SB.PCI0.B0D4, 0x09, Package () {0x00070000,1}},          // Package domain, Power Control Turbo Disable
        Package() {50, \_SB.PCI0.B0D4, 0x09, Package () {0x00030000,0x80000000}}, // Imax Controls - 30A 
        Package() {50, \_SB.PCI0.B0D4, 0x00, Package () {0x00040000,2}},          // Core control - 2 cores
        Package() {40, \_SB.PCI0.DPLY, 0x0A, Package () {0x00050000,60}},         // Display brightness - 60
        Package() {40, \_SB.PCI0.TMEM, 0x02, Package () {0x00010000,1000}},       // Memory domain, PL1, target power from TMEM.PPCC
        Package() {30, \_SB.PCI0.B0D4, 0x00, Package () {0x00020000,80}},         // System Agent Thermal Device, target frequency - P State 80%
        Package() {20, \_SB.PCI0.B0D4, 0x00, Package () {0x00020001,50}}          // System Agent Thermal Device, target frequency - T State 50%
      })

      If(CondRefOf(\_PR.CBMI)){
        Switch(ToInteger(\_PR.CBMI)){ // use the boot index from PPM to choose the Power Limit values and copy them to local variables
          case(0){
            If(LAnd(LGreaterEqual(\_PR.CLVL,1),LLessEqual(\_PR.CLVL,3))){ // copy PL0 values 
              Store (\_SB.PCI0.B0D4.CPNU(\_PR.PL20,1),Local0)               // PL2 minimum power
              Store (\_SB.PCI0.B0D4.MIN4(\_PR.PL10),Local1)                 // PL1 minimum power
            }
          }
          case(1){
            If(LOr(LEqual(\_PR.CLVL,2),LEqual(\_PR.CLVL,3))){ // copy PL1 values 
              Store (\_SB.PCI0.B0D4.CPNU(\_PR.PL21,1),Local0)   // PL2 minimum power
              Store (\_SB.PCI0.B0D4.MIN4(\_PR.PL11),Local1)     // PL1 minimum power
            }
          }
          case(2){
            If(LEqual(\_PR.CLVL,3)){                        // copy PL2 values 
              Store (\_SB.PCI0.B0D4.CPNU(\_PR.PL22,1),Local0) // PL2 minimum power
              Store (\_SB.PCI0.B0D4.MIN4(\_PR.PL12),Local1)   // PL1 minimum power
            }
          }
        }
      }

      // get the iMax value from the PCCC object and copy it to a local variable
      Store (DerefOf(Index(DerefOf(Index(\_SB.PCI0.B0D4.PCCX,1)),0)),Local2) // CurrentLimitMinimum

      // update the first three packages with the values that we had to dynamically determine
      // this section might have to be updated any time the PDRT is changed
      Store(Local0,Index(DeRefOf(Index (DeRefOf (Index (TMPD, 0)),3)),1)) // Package domain, PL2, target power from CPU PPM
      Store(Local1,Index(DeRefOf(Index (DeRefOf (Index (TMPD, 1)),3)),1)) // Package domain, PL1, target power from CPU PPM
      Store(Local2,Index(DeRefOf(Index (DeRefOf (Index (TMPD, 3)),3)),1)) // Imax Controls - 30A 

      Return(TMPD)
    } // Method(PDDC)

    Method(PDAC,,,,PkgObj)
    { 
      Name(TMPD,Package()
      { 
        Package() {100, \_SB.PCI0.B0D4, 0x09, Package () {0x00010001,0x80000000}}, // Package domain, PL2, target power from CPU PPM
        Package() {100, \_SB.PCI0.B0D4, 0x09, Package () {0x00010000,0x80000000}}, // Package domain, PL1, target power from CPU PPM
        Package() {100, \_SB.PCI0.B0D4, 0x09, Package () {0x00070000,0}},          // Package domain, Power Control Turbo Enable
        Package() {100, \_SB.PCI0.B0D4, 0x09, Package () {0x00030000,0x80000000}}, // Imax Controls - 30A 
        Package() {100, \_SB.PCI0.B0D4, 0x00, Package () {0x00040000,0x80000000}}, // Core control - cores from TCNT
        Package() {100, \_SB.PCI0.DPLY, 0x0A, Package () {0x00050000,100}},        // Display brightness - 100%
        Package() {100, \_SB.PCI0.TMEM, 0x02, Package () {0x00010000,5000}},       // Memory domain, PL1, target power from TMEM.PPCC
        Package() {100, \_SB.PCI0.B0D4, 0x00, Package () {0x00020000,100}}         // System Agent Thermal Device, target frequency - P State 0%
      })

      If(CondRefOf(\_PR.CBMI)){
        Switch(ToInteger(\_PR.CBMI)){ // use the boot index from PPM to choose the Power Limit values and copy them to local variables
          case(0){
            If(LAnd(LGreaterEqual(\_PR.CLVL,1),LLessEqual(\_PR.CLVL,3))){ // copy PL0 values 
              Store (\_SB.PCI0.B0D4.CPNU(\_PR.PL20,1),Local0)             // PL2 maximum power
              Store (\_SB.PCI0.B0D4.CPNU(\_PR.PL10,1),Local1)             // PL1 maximum power
            }
          }
          case(1){
            If(LOr(LEqual(\_PR.CLVL,2),LEqual(\_PR.CLVL,3))){ // copy PL1 values 
              Store (\_SB.PCI0.B0D4.CPNU(\_PR.PL21,1),Local0) // PL2 maximum power
              Store (\_SB.PCI0.B0D4.CPNU(\_PR.PL11,1),Local1) // PL1 maximum power
            }
          }
          case(2){
            If(LEqual(\_PR.CLVL,3)){                          // copy PL2 values 
              Store (\_SB.PCI0.B0D4.CPNU(\_PR.PL22,1),Local0) // PL2 maximum power
              Store (\_SB.PCI0.B0D4.CPNU(\_PR.PL12,1),Local1) // PL1 maximum power
            }
          }
        }
      }

      // get the iMax value from the PCCC object and copy it to a local variable
      Store (DerefOf(Index(DerefOf(Index(\_SB.PCI0.B0D4.PCCX,1)),1)),Local2) // CurrentLimitMaximum

      // update the first four packages with the values that we had to dynamically determine
      // this section might have to be updated any time the PDRT is changed
      Store(Local0,Index(DeRefOf(Index (DeRefOf (Index (TMPD, 0)),3)),1)) // Package domain, PL2, target power from CPU PPM
      Store(Local1,Index(DeRefOf(Index (DeRefOf (Index (TMPD, 1)),3)),1)) // Package domain, PL1, target power from CPU PPM
      Store(Local2,Index(DeRefOf(Index (DeRefOf (Index (TMPD, 3)),3)),1)) // Imax Controls - 30A 
      Store(TCNT,Index(DeRefOf(Index (DeRefOf (Index (TMPD, 4)),3)),1))   // Core control - cores from TCNT

      Return(TMPD)
    } // Method(PDAC)

  } // End IETM Device
} // End \_SB Scope

//
// EC support code
//
Scope(\_SB.PCI0.LPCB.H_EC) // Open scope to Embedded Controller
{
  //
  // Create a Mutex for PATx methods to prevent Sx resume race condition problems asscociated with EC commands.
  //
  Mutex(PATM, 0)

  // _QF1 (Query - Embedded Controller Query F1)
  //
  // Handler for EC generated SCI number F1.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(_QF1)
  { // Thermal sensor threshold crossing event handler
//[-start-130618-IB08050213-modify]//
    P8XH(0, 0xf1, 1)
//[-end-130618-IB08050213-modify]//
    Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSSR)), Local0)
    While(Local0) // Ensure that events occuring during execution
    {             // of this handler are not dropped
      \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.TSSR)) // clear all status bits
      If(And(Local0, 0x10, Local1))
      { // Memory temp threshold crossed
        Notify(\_SB.PCI0.TMEM, 0x90)
      }
      If(And(Local0, 0x8, Local1))
      { // Ambient temp threshold crossed
        Notify(\_SB.PCI0.LPCB.H_EC.TAMB, 0x90)
      }
      If(And(Local0, 0x4, Local1))
      { // Skin temp threshold crossed
        Notify(\_SB.PCI0.LPCB.H_EC.TSKN, 0x90)
      }
      If(And(Local0, 0x2, Local1))
      { // Fan temp threshold crossed
        Notify(\_SB.PCI0.LPCB.H_EC.TEFN, 0x90)
      }
      If(And(Local0, 0x01, Local1))
      { // VR temp threshold crossed
        Notify(\_SB.PCI0.LPCB.H_EC.T_VR, 0x90)
      }
      Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSSR)), Local0)
    }
  }
} // End \_SB.PCI0.LPCB.H_EC Scope

//
// Processor files.
//
  Include("B0d4Participant.asl")
  Include("B0d4DppmPolicy.asl")
  Include("B0d4CtdpPolicy.asl")
  Include("B0d4LpmPolicy.asl")

//
// CPU files
//
  Include("TcpuParticipant.asl")
  Include("TcpuDppmPolicy.asl")

//
// PCH files
// 
  Include("TpchParticipant.asl")
  Include("TpchDppmPolicy.asl")

//
// These participants, TAMB, TEFN, TFN1, TFN2, TMEM, TSKN, T_VR, are currently used by DPPM policies only. 
// Refer to the specific technology BIOS specification.
//
  Include("TAmbParticipant.asl")
  Include("TAmbDppmPolicy.asl")
  Include("TEfnParticipant.asl")
  Include("TEfnDppmPolicy.asl")
  Include("TFN1Participant.asl")
  Include("TFN2Participant.asl")
  Include("TMemParticipant.asl")
  Include("TMemDppmPolicy.asl")
  Include("TSknParticipant.asl")
  Include("TSknDppmPolicy.asl")
  Include("T_VRParticipant.asl")
  Include("T_VRDppmPolicy.asl")
  Include("DplyParticipant.asl")
  Include("TPwrParticipant.asl")
  Include("WWANParticipant.asl")
  Include("WWANDppmPolicy.asl")

//
// DPPM support files
//
  Include("Ertm.asl")
  Include("Trt.asl")
  Include("Art.asl")
  Include("Dppm.asl")


} // End SSDT
