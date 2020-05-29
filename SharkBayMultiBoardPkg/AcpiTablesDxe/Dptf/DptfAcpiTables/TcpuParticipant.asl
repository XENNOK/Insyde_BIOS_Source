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

  TcpuParticipant.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/


Scope(\_SB)
{
  Device(TCPU)
  {
    External (PPCS)

    Name(_HID, EISAID("INT3401"))  // Intel Dptf Processor Device, 1 per package
    Name(_UID,0)

    Name(CINT,1)
    Name(LSTM,0)  // Last temperature reported
    
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
      If (LEqual(SADE,2)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // PGMB (Pci GMch Bar)
    //
    // This object is required to be implemented on platforms supporting Intel code name Sandy Bridge processor. 
    // This object is not required for platforms supporting Intel code name Ivy Bridge processor.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Return MCH Bar value.
    //
    Method(PGMB, 0)
    {
      Return(\_SB.PCI0.MHBR)  // Return MCH Bar value
    }

    // _PPC (Performance Present Capabilities)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the range of states supported
    //   0 - States 0 through nth state are available (all states available)
    //   1 - States 1 through nth state are available
    //   2 - States 2 through nth state are available
    //   ...
    //   n - State n is available only
    //
    Name(_PPC,0)

    // SPPC (Set Participant Performance Capability)
    //
    // SPPC is a control method object that takes one integer parameter that will indicate the maximum allowable 
    // P-State for OSPM to use at any given time.
    //
    // Arguments: (1)
    //   Arg0 - integer
    // Return Value:
    //   None
    //
    Method(SPPC,1,Serialized)
    {
      Store(Arg0, \_PR.CPU0._PPC) // Note: CPU0._PPC is an Integer not a Method

      If(CondRefOf(\_SB.PCCD.PENB)) { // is CPPC enabled in SETUP?
        Notify(\_SB.PCCD,0x82) // CPPC notify
      } Else {
        Switch(ToInteger(TCNT)){
          Case(8){
            Notify(\_PR.CPU0, 0x80)  // Tell CPU0 driver to re-eval _PPC
            Notify(\_PR.CPU1, 0x80)  // Tell CPU1 driver to re-eval _PPC
            Notify(\_PR.CPU2, 0x80)  // Tell CPU2 driver to re-eval _PPC
            Notify(\_PR.CPU3, 0x80)  // Tell CPU3 driver to re-eval _PPC
            Notify(\_PR.CPU4, 0x80)  // Tell CPU4 driver to re-eval _PPC
            Notify(\_PR.CPU5, 0x80)  // Tell CPU5 driver to re-eval _PPC
            Notify(\_PR.CPU6, 0x80)  // Tell CPU6 driver to re-eval _PPC
            Notify(\_PR.CPU7, 0x80)  // Tell CPU7 driver to re-eval _PPC
          }
          Case(4){
            Notify(\_PR.CPU0, 0x80)  // Tell CPU0 driver to re-eval _PPC
            Notify(\_PR.CPU1, 0x80)  // Tell CPU1 driver to re-eval _PPC
            Notify(\_PR.CPU2, 0x80)  // Tell CPU2 driver to re-eval _PPC
            Notify(\_PR.CPU3, 0x80)  // Tell CPU3 driver to re-eval _PPC
          }
          Case(2){
            Notify(\_PR.CPU0, 0x80)  // Tell CPU0 driver to re-eval _PPC
            Notify(\_PR.CPU1, 0x80)  // Tell CPU1 driver to re-eval _PPC
          }
          Default{
            Notify(\_PR.CPU0, 0x80)  // Tell CPU0 driver to re-eval _PPC
          }
        }
      }
    }

    // _TMP (Temperature)
    //
    // This control method returns the thermal zone's current operating temperature.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the current temperature of the thermal zone (in tenths of degrees Kelvin)
    //
    Method(_TMP)
    {
      // Return the temperature to the OS if EC access is enabled.
      If(\ECON)
      {
        Return(\_SB.IETM.CTOK(\_SB.PCI0.LPCB.H_EC.PECH)) 
      }
      Else
      {
        // Return a static value as EC access is disabled or DPTF is active.
        Return(3000)
      }
    }

    // _DTI (Device Temperature Indication)
    //
    // Conveys the temperature of a device's internal temperature sensor to the platform when a temperature trip point
    // is crossed or when a meaningful temperature change occurs.
    //
    // Arguments: (1)
    //   Arg0 - An Integer containing the current value of the temperature sensor (in tenths Kelvin)
    // Return Value:
    //   None
    //
    Method(_DTI, 1)
    {
      Store(Arg0,LSTM)
      Notify(TCPU, 0x91) // notify the participant of a trip point change event
    }

    // _NTT (Notification Temperature Threshold)
    //
    // Returns the temperature change threshold for devices containing native temperature sensors to cause
    // evaluation of the _DTI object 
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the temperature threshold in tenths of degrees Kelvin.
    //
    Method(_NTT, 0)
    {
      Return(2782)  // 5 degree Celcius, this could be a platform policy with setup item
    }
    
    // _PSS (Performance Supported States)
    //
    // This optional object indicates to OSPM the number of supported processor performance states that any given system can support.
    //
    // Arguments: (1)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Pstate sub-packages as described below
    //
    // Return Value Information
    //   Package {
    //   PState [0] // Package - Performance state 0
    //   ....
    //   PState [n] // Package - Performance state n
    //   }
    //
    //   Each Pstate sub-Package contains the elements described below:
    //   Package {
    //     CoreFrequency     // Integer (DWORD)
    //     Power             // Integer (DWORD)
    //     Latency           // Integer (DWORD)
    //     BusMasterLatency  // Integer (DWORD)
    //     Control           // Integer (DWORD)
    //     Status            // Integer (DWORD)
    //   }
    //
    // Stub for the Actual CPU _PSS method.
    //
    Method(_PSS,,,,PkgObj)
    {
      If(CondRefOf(\_PR.CPU0._PSS))
      { // Ensure _PSS is present
      Return(\_PR.CPU0._PSS())
      } Else {
        Return(Package(){
          Package(){0,0,0,0,0,0},
          Package(){0,0,0,0,0,0}})
      }
    }

    // _TSS (Throttling Supported States)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Tstate sub-packages as described below
    //
    // Return Value Information
    //   Package {
    //   TState [0] // Package - Throttling state 0
    //   ....
    //   TState [n] // Package - Throttling state n
    //   }
    //
    //   Each Tstate sub-Package contains the elements described below:
    //   Package {
    //     Percent // Integer (DWORD)
    //     Power   // Integer (DWORD)
    //     Latency // Integer (DWORD)
    //     Control // Integer (DWORD)
    //     Status  // Integer (DWORD)
    //   }
    //
    // Stub for the Actual CPU _TSS method.
    //
    Method(_TSS,,,,PkgObj)
    {
      If(CondRefOf(\_PR.CPU0._TSS))
      { // Ensure _TSS is present
        Return(\_PR.CPU0._TSS())
      } Else {
        Return(Package(){
          Package(){0,0,0,0,0},
          Package(){0,0,0,0,0}})
      }
    }

    // _TPC (Throttling Present Capabilities)
    //
    // This optional object is a method that dynamically indicates to OSPM the number of throttling states currently supported by the platform.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the number of states supported:
    //   0 - states 0 .. nth state available (all states available)
    //   1 - state 1 .. nth state available
    //   2 - state 2 .. nth state available
    //   ...
    //   n - state n available only
    //
    Method(_TPC)
    {
      If(CondRefOf(\_PR.CPU0._TPC))
      { // Ensure _TPC is present
        Return(\_PR.CPU0._TPC)
      } Else {
        Return(0)
      }
    }

    // _PTC (Processor Throttling Control)
    //
    // _PTC is an optional object that defines a processor throttling control interface alternative to the I/O address spaced-based P_BLK throttling control register (P_CNT)
    //
    //  PDC0[2] = ACPI object indicating if OSPM is capable of direct access to On Demand throttling MSR
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A Package as described below
    //
    // Return Value Information
    //   Package {
    //     ControlRegister // Buffer (Resource Descriptor)
    //     StatusRegister // Buffer (Resource Descriptor)
    //   }
    //
    Method(_PTC,,,,PkgObj)
    {
      If(LAnd(CondRefOf(\PDC0),LNotEqual(\PDC0,0x80000000))) // is object present and initialized?
      {
        If(And(\PDC0, 0x0004)) { // does OS support MSR interface?
          Return(Package() {
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)} }) // if OS supports MSR interface
        } Else {
          Return(Package() {
          ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)},
          ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)} }) // if OS support IO based interface
        }
      } Else {
        Return(Package() {
        ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
        ResourceTemplate(){Register(FFixedHW, 0, 0, 0)} }) // if object is not present or not initialized then return MSR interface
      }
    }

    // _TSD (T-State Dependency)
    //
    // This optional object provides T-state control cross logical processor dependency information to OSPM.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of T-state dependency Packages as described below.
    //
    // Return Value Information
    //   Package {
    //     NumEntries    // Integer
    //     Revision      // Integer (BYTE)
    //     Domain        // Integer (DWORD)
    //     CoordType     // Integer (DWORD)
    //     NumProcessors // Integer (DWORD)
    //   }
    //
    Method(_TSD,,,,PkgObj)
    {
      If(CondRefOf(\_PR.CPU0._TSD))
      { // Ensure _TSD is present
        Return(\_PR.CPU0._TSD())
      } Else {
        Return(Package(){
          Package(){5,0,0,0,0},
          Package(){5,0,0,0,0}})
      }
    }

    // _TDL (T-state Depth Limit)
    //
    // This optional object evaluates to the _TSS entry number of the lowest power throttling state that OSPM may use.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the Throttling Depth Limit _TSS entry number:
    //   0 - throttling disabled.
    //   1 - state 1 is the lowest power T-state available.
    //   2 - state 2 is the lowest power T-state available.
    //   ...
    //   n - state n is the lowest power T-state available.
    //
    Method(_TDL)
    {
      If(LAnd(CondRefOf(\_PR.CPU0._TSS),CondRefOf(\_PR.CFGD)))
      { // Ensure _TSS is present
        If(And(\_PR.CFGD, PPM_TSTATE_FINE_GRAINED))
        {
          Return(Subtract(SizeOf(\_PR.CPU0.TSMF),1))
        } Else {
          Return(Subtract(SizeOf(\_PR.CPU0.TSMC),1))
        }
      } Else {
        Return(0)
      }
    }

    // _PDL (P-state Depth Limit)
    //
    // This optional object evaluates to the _PSS entry number of the lowest performance P-state that OSPM may use when performing passive thermal control.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the P-state Depth Limit _PSS entry number:
    //   Integer containing the P-state Depth Limit _PSS entry number:
    //   0 - P0 is the only P-state available for OSPM use
    //   1 - state 1 is the lowest power P-state available
    //   2 - state 2 is the lowest power P-state available
    //   ...
    //   n - state n is the lowest power P-state available
    //
    Method(_PDL)
    {
      If(CondRefOf(\_PR.CPU0._PSS))
      { // Ensure _PSS is present
        Return(Subtract(SizeOf(\_PR.CPU0._PSS),1))
      } Else {
        Return(0)
      }
    }

  } // End Device(TCPU)
} // End Scope(\_SB)

