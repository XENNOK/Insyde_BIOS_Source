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

  Cppc.asl

Abstract:

  Intel ACPI Reference Code for ACPI 5.0 Collaborative Processor Performance Control (CPPC) 

--*/

#include "PowerMgmtDefinitions.h"

DefinitionBlock (
	"Cppc.aml",
	"SSDT",
	1,
	"CppcTabl",
	"CppcTabl",
	0x1000 )
{

External(TCNT, IntObj)
External(OSYS, IntObj)
External(\PDC0, IntObj)
External(\_PR.CPU0, ProcessorObj)
External(\_PR.CPU0._PSS, MethodObj)
External(\_PR.CPU0._PPC, IntObj)
External(\_PR.CPU0._CPC, MethodObj)
External(\_PR.CPU0._TSS, MethodObj)
External(\_PR.CPU1, ProcessorObj)
External(\_PR.CPU2, ProcessorObj)
External(\_PR.CPU3, ProcessorObj)
External(\_PR.CPU4, ProcessorObj)
External(\_PR.CPU5, ProcessorObj)
External(\_PR.CPU6, ProcessorObj)
External(\_PR.CPU7, ProcessorObj)

Scope(\_SB)
{
// Define PCCD device so that we can load CPPC driver against this 
// device.  This driver will process PCC commands from OSPM in 
// PCC Shared memory region.
Device(PCCD)
{

//
// These objects are patched during POST.
// 
Name(PCCA, 0x80000000) // Address of PCC shared memory buffer, fixed up during POST
Name(PCCS, 0x80000000) // PCCA buffer size
Name(PENB, 0x80000000) // REQUIRED object.  CPCC Enable/Disable. 1 = Enable, 0 = Disable. Also used by _OSC evaluation of CPPC support.

Name(_HID, EISAID("INT340F")) 
Name(_STR, Unicode ("Collaborative Processor Performance Control (CPPC)"))

  // _STA (Status)
  //
  // This object returns the current status of a device.
  //   Checks if Win8 or later is present.
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
    If(LGreaterEqual(OSYS,2012)){
      Return(0x0F)
    } Else {
      Return(0x00)
    }
  }

  Name (TMPP, Package (){0x80000000,0x80000000})

  // PADR (Pcc shared memory ADRess)
  //
  // This ACPI object evaluates to a package, containing the PCC Shared Memory 32-bit address and its size in bytes.  
  // This object will be evaluated by the Intel CPPC driver during its initialization.  This object is REQUIRED.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   A package of 2 Integer (DWORD) elements:  PCC Shared Memory Address and PCC Shared Memory Length.
  //
  Method(PADR)
  {
    Store(PCCA,Index (TMPP, 0))
    Store(PCCS,Index (TMPP, 1))
    Return(TMPP)
  }

  // GPRN (Guaranteed Performance Register Notification)
  //
  // The purpose of this object is to notify OSPM on changes to CPPC Guaranteed Performance register. This object is REQUIRED.
  // This object is evaluated by the CPPC driver dynamically whenever the CPPC driver changes the CPPC Guaranteed Performance register value. 
  // Upon evaluation, GPRN control method issues an ACPI notification on each processor device with notify code = 0x83 and 
  // OSPM processes the ACPI notification. 
  //	
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(GPRN,0,Serialized)
  {
      Switch(ToInteger(TCNT)){
        Case(8){
          Notify(\_PR.CPU0, 0x83)
          Notify(\_PR.CPU1, 0x83)
          Notify(\_PR.CPU2, 0x83)
          Notify(\_PR.CPU3, 0x83)
          Notify(\_PR.CPU4, 0x83)
          Notify(\_PR.CPU5, 0x83)
          Notify(\_PR.CPU6, 0x83)
          Notify(\_PR.CPU7, 0x83)
        }
        Case(4){
          Notify(\_PR.CPU0, 0x83)
          Notify(\_PR.CPU1, 0x83)
          Notify(\_PR.CPU2, 0x83)
          Notify(\_PR.CPU3, 0x83)
        }
        Case(2){
          Notify(\_PR.CPU0, 0x83)
          Notify(\_PR.CPU1, 0x83)
        }
        Default{
          Notify(\_PR.CPU0, 0x83)
        }
      }
  }

  // PCFG (Platform Configuration)
  //
  // The purpose of this object is for platform to enable or disable EE P-States selection of frequency below P1. 
  // This method is REQUIRED if platform issues ACPI notification code 0x81 on CPPC device.
  // This object is evaluated by the CPPC driver once during boot (to get boot-time selection) and dynamically 
  // whenever platform issues an ACPI notification (notify code = 0x81) to change optimization selection. 
  //	
  // Arguments: (0)
  //   None
  // Return Value:
  //   An Integer containing CPPC optimization enables bitmap:
  //     Bit     Value	Description
  //     [0]     0      CPPC driver determines (based on OS power plan settings) when to enable EE P-States selection of frequency below P1.
  //     [0]     1      Disable EE P-States selection of frequency below P1.
  //     [31:1]  0      Reserved for future use
  //
  Name(PCFG,0)

  // _PTC (Processor Throttling Control)
  //
  // _PTC defines the processor throttling control interface.
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
  Method(_PPC)
  {
    If(CondRefOf(\_PR.CPU0._PPC))
    { // Ensure _PPC is present
      Return(\_PR.CPU0._PPC)
    } Else {
      Return(0)
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


} // End Device(PCCD)
} // End Scope(_SB)

}
