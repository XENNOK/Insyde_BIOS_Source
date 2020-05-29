/** @file

  Intel Processor Power Management ACPI Code

  WARNING: You are authorized and licensed to install and use this BIOS code
  ONLY on an IST PC. This utility may damage any system that does not
  meet these requirements.

    An IST PC is a computer which
    (1) Is capable of seamlessly and automatically transitioning among
    multiple performance states (potentially operating at different
    efficiency ratings) based upon power source changes, END user
    preference, processor performance demand, and thermal conditions; and
    (2) Includes an Intel Pentium II processors, Intel Pentium III
    processor, Mobile Intel Pentium III Processor-M, Mobile Intel Pentium 4
    Processor-M, Intel Pentium M Processor, or any other future Intel
    processors that incorporates the capability to transition between
    different performance states by altering some, or any combination of,
    the following processor attributes: core voltage, core frequency, bus
    frequency, number of processor cores available, or any other attribute
    that changes the efficiency (instructions/unit time-power) at which the
    processor operates.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c)  1999 - 2011 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

*/

#include <PowerMgmtDefinitions.h>

DefinitionBlock(
  "CPU0TST.aml",
  "SSDT",
  0x01,
  "PmRef",
  "Cpu0Tst",
  0x3000
  )
{
  External(\_PR.CPU0, DeviceObj)
  External(PDC0)
  External(CFGD)
  External(_PSS)
  External (TCNT)

  Scope(\_PR.CPU0)
  {
//[-start-130328-IB10310017-modify]//
#if SPS_ME_SUPPORT
    Method(_TPC, 0)
    {
      External(\PETE, IntObj)
      External(\TSLM, IntObj)
      Or(\PETE, 0x40, \PETE) // Confirm T-States support on this system
      Return(\TSLM)          // Return max T-States available
    }
#else
    Name(_TPC, 0)  // All T-States are available
#endif
//[-end-130328-IB10310017-modify]//

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      //
      // IF OSPM is capable of direct access to MSR
      //    Report MSR interface
      // ELSE
      //    Report I/O interface
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //
      If(And(PDC0, 0x0004)) {
 
        Return(Package() {
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)}
        })
      }

      Return(Package() {
        ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)},
        ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)}
      })
    }

    //
    // _TSS package for fine-grained T-State control. 
    // "Power" fields are replaced with real values by the first
    // call of _TSS method.
    //
    Name(TSMF, Package() {
        Package(){100, 1000, 0, 0x00, 0},
        Package(){ 94,  940, 0, 0x1F, 0},
        Package(){ 88,  880, 0, 0x1E, 0},
        Package(){ 82,  820, 0, 0x1D, 0},
        Package(){ 75,  760, 0, 0x1C, 0},
        Package(){ 69,  700, 0, 0x1B, 0},
        Package(){ 63,  640, 0, 0x1A, 0},
        Package(){ 57,  580, 0, 0x19, 0},
        Package(){ 50,  520, 0, 0x18, 0},
        Package(){ 44,  460, 0, 0x17, 0},
        Package(){ 38,  400, 0, 0x16, 0},
        Package(){ 32,  340, 0, 0x15, 0},
        Package(){ 25,  280, 0, 0x14, 0},
        Package(){ 19,  220, 0, 0x13, 0},
        Package(){ 13,  160, 0, 0x12, 0},
        Package(){  7,  100, 0, 0x11, 0}
    })

    //
    // _TSS package for T-State control (Coarse grained)
    // "Power" fields are replaced with real values by the first
    // call of _TSS method.
    //
    Name(TSMC, Package() {
        Package(){100, 1000, 0, 0x00, 0},
        Package(){ 88,  875, 0, 0x1E, 0},
        Package(){ 75,  750, 0, 0x1C, 0},
        Package(){ 63,  625, 0, 0x1A, 0},
        Package(){ 50,  500, 0, 0x18, 0},
        Package(){ 38,  375, 0, 0x16, 0},
        Package(){ 25,  250, 0, 0x14, 0},
        Package(){ 13,  125, 0, 0x12, 0}
    })

    Name(TSSF, 0)  // Flag for TSIF/TSIC/TSMF/TSMC initialization
    Mutex(TSMO, 0) // Mutex object to ensure the _TSS initalization code is only executed once
    Method(_TSS, 0)
    {
      //
      // Update "Power" fields of TSIC or TSIF or TSMC or TSMF with the LFM
      // power data IF _PSS is available
      //    Power caluclation: 
      //               n - Number of T-states available
      //              _TSS(x).power = LFM.Power * (n-x)/n
      //
      IF (LAnd(LNot(TSSF),CondRefOf(_PSS)))
      {
        //
        // Acquire Mutex to make sure the initialization happens only once.  
        //
        Acquire (TSMO, 0xFFFF)
        //
        // Only one thread will be able to acquire the mutex at a time, but the other threads which have acquired the mutex previously, will eventually try to execute the TSS initalization code. 
        // So, let's check if TSS has already been initalized once again. If its initalized, skip the initalization.
        //
        IF (LAnd(LNot(TSSF),CondRefOf(_PSS))) 
        {
          Name ( LFMI, 0)  
          Store (SizeOf(_PSS), LFMI)
          Decrement(LFMI)    // Index of LFM entry in _PSS
          Name ( LFMP, 0) //LFM Power from _PSS
          Store ( DerefOf(Index(DerefOf(Index(_PSS,LFMI)),1)) , LFMP)
          Store (0, Local0)
          
          //
          // Copy reference of appropiate TSS package based on Fine grained T-state support
          // We'll update the power in the package directly (via the reference variable Local1)
          //
          // If Fine Grained T-states is enabled 
          //      TSMF
          //    ELSE
          //      TSMC
          //
          If(And(CFGD,PPM_TSTATE_FINE_GRAINED))
          {
            Store ( RefOf(TSMF), Local1 )
            Store ( SizeOf(TSMF),Local2 )
          }
          Else 
          {
            Store ( RefOf(TSMC), Local1 )
            Store ( SizeOf(TSMC),Local2 )
          }
  
          While(LLess(Local0, Local2))
          {
            Store(Divide(Multiply(LFMP, Subtract(Local2, Local0)), Local2),
                  Local4)    // Power for this entry
            Store(Local4,Index(DerefOf(Index(DerefOf(Local1),Local0)),1))
            Increment(Local0)
          }
          
          Store(Ones, TSSF)    // Set flag to indicate TSS table initalization is complete
        }
        
        Release (TSMO)

      }
      //
      //  If Fine Grained T-states is enabled 
      //    Report TSMF
      //  ELSE
      //    Report TSMC
      //
      If(And(CFGD, PPM_TSTATE_FINE_GRAINED))
      {
        Return(TSMF)
      }
      Else 
      {
        Return(TSMC)
      }
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF !(direct access to MSR)
      //    Report SW_ANY as the coordination type
      // ELSE
      //   Report SW_ALL as the coordination type
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //
      If (LNot(And(PDC0,4))) {
        Return(Package(){     // SW_ANY
          Package(){
            5,                // # entries.
            0,                // Revision.
            0,                // Domain #.
            0xFD,             // Coord Type- SW_ANY
            TCNT              // # processors.
          }
        })
      }
      Return(Package(){       // SW_ALL
        Package(){
          5,                  // # entries.
          0,                  // Revision.
          0,                  // Domain #.
          0xFC,               // Coord Type- SW_ALL
          1                   // # processors.
        }
      })
    }
  }
} // End of Definition Block

