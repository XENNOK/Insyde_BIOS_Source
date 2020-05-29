/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  Ctdp.asl - Enable CTDP without using a driver or EC, i.e. this is the BIOS only solution.

Abstract:

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

--*/

DefinitionBlock (
  "CTDP.aml",
  "SSDT",
  0x01,
  "CtdpB",
  "CtdpB",
  0x1000
  )
{

External(TCNT, IntObj)
External(PNHM, IntObj)
External(\_SB.PCI0, DeviceObj)
External(\_SB.PCI0.MHBR, FieldUnitObj)
External(\_PR.CPU0, DeviceObj)
External(\_PR.CPU1, DeviceObj)
External(\_PR.CPU2, DeviceObj)
External(\_PR.CPU3, DeviceObj)
External(\_PR.CPU4, DeviceObj)
External(\_PR.CPU5, DeviceObj)
External(\_PR.CPU6, DeviceObj)
External(\_PR.CPU7, DeviceObj)
External(\_PR.CPU0._PPC, IntObj)
External(\_PR.CPU0._PSS, MethodObj)
External(\_SB.PCCD.PENB, IntObj)

Scope(\_SB.PCI0)
{
  //
  // Memory window to the CTDP registers starting at MCHBAR+5000h. 
  //
  OperationRegion (MBAR, SystemMemory, Add(ShiftLeft(MHBR,15),0x5000), 0x1000)
    Field (MBAR, ByteAcc, NoLock, Preserve)
    {
      Offset (0x930), // PACKAGE_POWER_SKU (MCHBAR+0x5930)
      PTDP, 15,       // TDP Package Power [14:0]
      ,      1,       // reserved [15]
      PMIN, 15,       // Minimal Package Power [30:16]
      ,      1,       // Reserved [31]
      PMAX, 15,       // Maximal Package Power [46:32]
      ,      1,       // Reserved [47]
      TMAX,  7,       // Maximal Time Window [54:48]
      Offset (0x938), // PACKAGE_POWER_SKU_UNIT (MCHBAR+0x5938)
      PWRU,  4,       // Power Units [3:0]
      ,      4,       // Reserved [7:4]
      EGYU,  5,       // Energy Units [12:8]
      ,      3,       // Reserved [15:13]
      TIMU,  4,       // Time Units [19:16]
      Offset (0x958), // PLATFORM_INFO (MCHBAR+0x5958)
          , 32,       // [31:0]
      LPMS,  1,       // LPM Support [32]
      CTNL,  2,       // CONFIG_TDP_NUM_LEVELS [34:33]
      Offset (0x9A0), // TURBO_POWER_LIMIT1 (MCHBAR+0x59A0)
      PPL1, 15,       // PKG_PWR_LIM_1 [14:0]
      PL1E,1,         // PKG_PWR_LIM1_EN [15]
      ,      1,       // reserved [16]
      PL1T, 7,        // PKG_PWR_LIM_1_TIME [23:17]
      Offset (0x9A4), // TURBO_POWER_LIMIT2 (MCHBAR+0x59A4)
      PPL2, 15,       // PKG_PWR_LIM_2 [14:0]
      PL2E,1,         // PKG_PWR_LIM2_EN [15]
      ,      1,       // reserved [16]
      PL2T, 7,        // PKG_PWR_LIM_2_TIME [23:17]
      Offset (0xF3C), // CONFIG_TDP_NOMINAL (MCHBAR+0x5F3C)
      TARN,  8,       // TDP Ratio [7:0]
      Offset (0xF40), // CONFIG_TDP_LEVEL1 (MCHBAR+0x5F40)
      PTD1, 15,       // Package TDP [14:0]
      ,      1,       // reserved [15]
      TAR1,  8,       // TDP Ratio [23:16]
      ,      8,       // reserved [31:24]
      PMX1, 15,       // Package MAX Power [46:32]
      ,      1,       // reserved [47]
      PMN1, 15,       // Package MIN Power [62:48]
      Offset (0xF48), // CONFIG_TDP_LEVEL2 (MCHBAR+0x5F48)
      PTD2, 15,       // Package TDP [14:0]
      ,      1,       // reserved [15]
      TAR2,  8,       // TDP Ratio [23:16]
      ,      8,       // reserved [31:24]
      PMX2, 15,       // Package MAX Power [46:32]
      ,      1,       // reserved [47]
      PMN2, 15,       // Package MIN Power [62:48]
      Offset (0xF50), // CONFIG_TDP_CONTROL (MCHBAR+0x5F50)
      CTCL,  2,       // TDP Level [1:0]
      ,     29,       // reserved [30:2]
      CLCK,  1,       // Config TDP Lock [31]
      Offset (0xF54), // TURBO_ACTIVATION_RATIO (MCHBAR+0x5F54)
      TAR_,  8,       // Max Non Turbo Ratio [7:0]
    }

  // CTCU (Config Tdp Control Up)
  // 
  // Program the CTDP Up point.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(CTCU)
  {
    Store(PTD2,PPL1)       // Set PL1
    Store(1,PL1E)          // Set PL1 enable
    Store(CLC2(PTD2),PPL2) // Set PL2
    Store(1,PL2E)          // Set PL2 enable
    SPPC(1)                // Set _PPC
    Subtract(TAR2,1,TAR_)  // Set TAR
    Store(2,CTCL)          // Set CTC
  }

  // CTCN (Config Tdp Control Nominal)
  //
  // Program the CTDP Nominal point.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(CTCN)
  {
    If(LEqual(CTCL,1))       // algorithm for going to Nominal from Down
    {
      Store(PTDP,PPL1)       // Set PL1
      Store(1,PL1E)          // Set PL1 enable
      Store(CLC2(PTDP),PPL2) // Set PL2
      Store(1,PL2E)          // Set PL2 enable
      NPPC(TARN)             // Set _PPC
      Subtract(TARN,1,TAR_)  // Set TAR
      Store(0,CTCL)          // Set CTC
    }
    ElseIf(LEqual(CTCL,2))   // algorithm for going to Nominal from Up
    {
      Store(0,CTCL)          // Set CTC
      Subtract(TARN,1,TAR_)  // Set TAR
      NPPC(TARN)             // Set _PPC
      Store(CLC2(PTDP),PPL2) // Set PL2
      Store(1,PL2E)          // Set PL2 enable
      Store(PTDP,PPL1)       // Set PL1
      Store(1,PL1E)          // Set PL1 enable
    }
  }

  // CTCD (Config Tdp Control Down)
  // 
  // Program the CTDP Down point.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(CTCD)
  {
    Store(1,CTCL)          // Set CTC
    Subtract(TAR1,1,TAR_)  // Set TAR
    NPPC(TAR1)             // Set _PPC
    Store(CLC2(PTD1),PPL2) // Set PL2
    Store(1,PL2E)          // Set PL2 enable
    Store(PTD1,PPL1)       // Set PL1
    Store(1,PL1E)          // Set PL1 enable
  }

  // NPPC (Notify _PPC object)
  //
  // Find the ratio or next highest ratio in the _PSS table and program _PPC with the index of that ratio.
  //
  // Arguments: (1)
  //   Arg0 - Turbo Activation Ratio
  // Return Value:
  //   None
  //
  Method(NPPC,1)
  {
      Name(TRAT,0)     // holder for the target ratio
      Name(PRAT,0)     // holder for the ratio in _PSS table
      Name(TMPI,0)     // index 
      Store(Arg0,TRAT) // init target ratio from caller
      Store(SizeOf(\_PR.CPU0._PSS),TMPI) // init index from _PSS

      While(LNotEqual(TMPI,0)){
        Decrement(TMPI) // convert from 1 based count to 0 based count
        Store(DeRefOf(Index(DeRefOf(Index(\_PR.CPU0._PSS, TMPI)),4)),PRAT)
        ShiftRight(PRAT,8,PRAT)
        If(LGreaterEqual(PRAT,TRAT)){
          SPPC(TMPI)
          Break
        }
      }
  }

  // SPPC (Set Participant Performance Capability)
  //
  // Progam the _PPC object and notify the OSPM. 
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

  // CLC2 (CaLCulate PL2)
  //
  // IF Haswell Traditional THEN multiply PL1 by 1.25 to get PL2.
  // ELSE IF Haswell ULT THEN use 25 watts as the PL2.
  //
  // Arguments: (1)
  //   Arg0 - integer
  // Return Value:
  //   integer
  //
  Method(CLC2,1)
  {
    And(PNHM,0x0FFF0FF0,Local0) // remove stepping from CPUID
    Switch(Local0){
      Case(0x000306C0){ // Haswell Traditional
        Return(Divide(Multiply(Arg0,5),4)) // Multiply a number by 1.25
      }
      Case(0x00040650){ // Haswell ULT
        Return(Multiply(25,8))
      }
      Default{
        Return(Divide(Multiply(Arg0,5),4)) // Multiply a number by 1.25
      }
    }
  }

} // end of scope(\_SB.PCI0)
} // end of definition block
