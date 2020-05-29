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

  B0d4Participant.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/


Scope(\_SB.PCI0)
{
  Device(B0D4)  // SA Thermal Device
  {
    Name(_ADR, 0x00040000)

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
      If (LEqual(SADE,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    //
    // Define an OpRegion for the DPTF MSR's accessed via MCHBAR+0x5000
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
        Offset (0x998), // RP_STATE_CAP_0_0_0_MCHBAR_PCU (MCHBAR+0x5998)
        RP0C,  8,       // [7:0] RP0_CAP,  These fields indicate the maximum RPx base frequency capability for the Integrated GFX Engine (GT).
        RP1C,  8,       // [15:8] RP1_CAP,  Values are in units of 100 MHz.
        RPNC,  8,       // [23:16] RPN_CAP
        Offset (0xF3C), // CONFIG_TDP_NOMINAL (MCHBAR+0x5F3C)
        TRAT,  8,       // TDP Ratio [7:0]
        Offset (0xF40), // CONFIG_TDP_LEVEL1 (MCHBAR+0x5F40)
        PTD1, 15,       // Package TDP [14:0]
        ,      1,       // reserved [15]
        TRA1,  8,       // TDP Ratio [23:16]
        ,      8,       // reserved [31:24]
        PMX1, 15,       // Package MAX Power [46:32]
        ,      1,       // reserved [47]
        PMN1, 15,       // Package MIN Power [62:48]
        Offset (0xF48), // CONFIG_TDP_LEVEL2 (MCHBAR+0x5F48)
        PTD2, 15,       // Package TDP [14:0]
        ,      1,       // reserved [15]
        TRA2,  8,       // TDP Ratio [23:16]
        ,      8,       // reserved [31:24]
        PMX2, 15,       // Package MAX Power [46:32]
        ,      1,       // reserved [47]
        PMN2, 15,       // Package MIN Power [62:48]
        Offset (0xF50), // CONFIG_TDP_CONTROL (MCHBAR+0x5F50)
        CTCL,  2,       // TDP Level [1:0]
        ,     29,       // reserved [30:2]
        CLCK,  1,       // Config TDP Lock [31]
        Offset (0xF54), // TURBO_ACTIVATION_RATIO (MCHBAR+0x5F54)
        MNTR,  8,       // Max Non Turbo Ratio [7:0]
      }

    Name(XPCC,0) // semaphore to record when PPCC gets called for the first time

    // PPCC (Participant Power Control Capabilities)
    //
    // The PPCC object evaluates to a package of packages that indicates to DPTF processor
    // participant the power control capabilities.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   PPCC package of packages
    //
    Method(PPCC,0,Serialized,,PkgObj)
    {
      If(LAnd(LEqual(XPCC,0),CondRefOf(\_PR.CBMI))){
        Switch(ToInteger(\_PR.CBMI)){ // use the boot index from PPM to choose the PL for PPCC
          case(0){
            If(LAnd(LGreaterEqual(\_PR.CLVL,1),LLessEqual(\_PR.CLVL,3))){
              CPL0() // copy PL0 values to PPCC
              Store(1,XPCC)
            }
          }
          case(1){
            If(LOr(LEqual(\_PR.CLVL,2),LEqual(\_PR.CLVL,3))){
              CPL1() // copy PL1 values to PPCC
              Store(1,XPCC)
            }
          }
          case(2){
            If(LEqual(\_PR.CLVL,3)){
              CPL2() // copy PL2 values to PPCC
              Store(1,XPCC)
            }
          }
        }
      }
      Return(NPCC)
    }

    // PPCC (Participant Power Control Capabilities)
    //
    // The PPCC object evaluates to a package of packages that indicates to DPTF processor
    // participant the power control capabilities.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   PPCC package of packages
    //
    // PPCC will be initialized by the _INI method with power on default values from the PPM code.
    //
    Name (NPCC, Package()
    {                          // Field Name : Field Type
      2,                       // Revision : DWordConst

      Package () 	       // Power Limit 1
      { 
        0,                     // PowerLimitIndex : DWordConst = 0
        35000,                 // PowerLimitMinimum : DWordConst
        45000,                 // PowerLimitMaximum : DWordConst
        28,                    // TimeWindowMinimum : DWordConst
        32,                    // TimeWindowMaximum : DWordConst
        1000,                  // StepSize : DWordConst
      },
      Package ()               // Power Limit 2
      { 
        1,                     // PowerLimitIndex : DWordConst = 1
        56250,                 // PowerLimitMinimum : DWordConst
        56250,                 // PowerLimitMaximum : DWordConst
        0,                     // TimeWindowMinimum : DWordConst
        0,                     // TimeWindowMaximum : DWordConst
        1000,                  // StepSize : DWordConst
      }
    }) // End of PPCC object


    // CPNU (Convert Power Number from MMIO register to correct Units)
    //
    // Arguments: (1)
    //   Arg0 = Number to be converted
    //   Arg1 = Units desired
    //     0 = Watts
    //     1 = MilliWatts
    // Return Value:
    //   Converted integer
    //
    Method(CPNU,2,Serialized,,IntObj)
    {
      Name(CNVT,0) // converted number
      Name(PPUU,0) // units
      Name(RMDR,0) // remainder

      if (LEqual(PWRU,0)) {  // use PACKAGE_POWER_SKU_UNIT - Power Units[3:0]
        Store(1,PPUU)
      } else {
        ShiftLeft(Decrement(PWRU),2,PPUU) // get units
      }
      Divide(Arg0,PPUU,RMDR,CNVT) // convert Arg0 to Watts

      if(LEqual(Arg1,0)){
        Return(CNVT)              // return in watts
      } else {
        Multiply(CNVT,1000,CNVT)  // convert to milliwatts
        Multiply(RMDR,1000,RMDR)  // convert remainder to a useful integer
        Divide(RMDR,PPUU,Local0,RMDR) // convert remainder to watts
        Add(CNVT,RMDR,CNVT)       // add the integer part and the fraction part together
        Return(CNVT)              // return in milliwatts
      }
    }

    // MIN4 (find next lower rounded multiple of 4)
    //
    // Arguments: (1)
    //   Number to be converted
    // Return Value:
    //   Converted integer
    //
    Method(MIN4,1,Serialized,,IntObj)
    {
      Name(RMDR,0) // remainder
      Name(DIVD,0) // dividend

      Store(CPNU(Arg0,0),DIVD)  // to convert Arg0 to Watts

      Subtract(DIVD,1,DIVD)
      If(LOr(LLess(DIVD,4),LEqual(DIVD,0))){
        Return(1)
      } Else{
        Divide(DIVD,4,RMDR)
      }

      While(LNotEqual(RMDR,0)){
        Subtract(DIVD,1,DIVD)
        If(LNotEqual(DIVD,0)){
          Divide(DIVD,4,RMDR)
        }
      }

      Return(Multiply(DIVD,1000)) // return in milliwatts
    }

    // MAX4 (find next higher rounded multiple of 4)
    //
    // Arguments: (1)
    //   Number to be converted
    // Return Value:
    //   Converted integer
    //
    Method(MAX4,1,Serialized,,IntObj)
    {
      Name(RMDR,0) // remainder
      Name(DIVD,0) // dividend

      Store(CPNU(Arg0,0),DIVD)  // to convert Arg0 to Watts

      Add(DIVD,1,DIVD)
      Divide(DIVD,4,RMDR)

      While(LNotEqual(RMDR,0)){
        Add(DIVD,1,DIVD)
        Divide(DIVD,4,RMDR)
      }

      Return(Multiply(DIVD,1000)) // return in milliwatts
    }

    // CPL0 (Copy PL0 power limits to PPCC)
    //
    // Arguments: 
    //   None
    // Return Value:
    //   None
    //
    Method(CPL0,0)
    {
      Store (2,Index(\_SB.PCI0.B0D4.NPCC,0))                                         // Revision
      Store (0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),0))                       // PowerLimitIndex
      If (LEqual(\MPLT, 0)){
        Store (MIN4(\_PR.PL10),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),1))       // PowerLimitMinimum
      } Else {
        Store (\MPLT,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),1))                 // PowerLimitMinimum
      }
      Store (CPNU(\_PR.PL10,1),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),2))       // PowerLimitMaximum
      Store (\_PR.PLW0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),3))               // TimeWindowMinimum
      Add (\_PR.PLW0,4,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),4))               // TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),5))                    // StepSize
      Store (1,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),0))                       // PowerLimitIndex
      Store (CPNU(\_PR.PL20,1),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),1))       // PowerLimitMinimum
      Store (CPNU(\_PR.PL20,1),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),2))       // PowerLimitMaximum
      Store (0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),3))                       // TimeWindowMinimum
      Store (0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),4))                       // TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),5))                    // StepSize
    }

    // CPL1 (Copy PL1 power limits to PPCC)
    //
    // Arguments: 
    //   None
    // Return Value:
    //   None
    //
    Method(CPL1,0)
    {
      Store (2,Index(\_SB.PCI0.B0D4.NPCC,0))                                         // Revision
      Store (0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),0))                       // PowerLimitIndex
      If (LEqual(\MPLT, 0)){
        Store (MIN4(\_PR.PL11),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),1))       // PowerLimitMinimum
      } Else {
        Store (\MPLT,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),1))                 // PowerLimitMinimum
      }
      Store (CPNU(\_PR.PL11,1),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),2))       // PowerLimitMaximum
      Store (\_PR.PLW1,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),3))               // TimeWindowMinimum
      Add (\_PR.PLW1,4,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),4))               // TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),5))                    // StepSize
      Store (1,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),0))                       // PowerLimitIndex
      Store (CPNU(\_PR.PL21,1),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),1))       // PowerLimitMinimum
      Store (CPNU(\_PR.PL21,1),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),2))       // PowerLimitMaximum
      Store (0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),3))                       // TimeWindowMinimum
      Store (0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),4))                       // TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),5))                    // StepSize
    }

    // CPL2 (Copy PL2 power limits to PPCC)
    //
    // Arguments: 
    //   None
    // Return Value:
    //   None
    //
    Method(CPL2,0)
    {
      Store (2,Index(\_SB.PCI0.B0D4.NPCC,0))                                         // Revision
      Store (0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),0))                       // PowerLimitIndex
      If (LEqual(\MPLT, 0)){
        Store (MIN4(\_PR.PL12),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),1))       // PowerLimitMinimum
      } Else {
        Store (\MPLT,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),1))                 // PowerLimitMinimum
      }
      Store (CPNU(\_PR.PL12,1),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),2))       // PowerLimitMaximum
      Store (\_PR.PLW2,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),3))               // TimeWindowMinimum
      Add (\_PR.PLW2,4,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),4))               // TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,1)),5))                    // StepSize
      Store (1,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),0))                       // PowerLimitIndex
      Store (CPNU(\_PR.PL22,1),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),1))       // PowerLimitMinimum
      Store (CPNU(\_PR.PL22,1),Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),2))       // PowerLimitMaximum
      Store (0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),3))                       // TimeWindowMinimum
      Store (0,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),4))                       // TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PCI0.B0D4.NPCC,2)),5))                    // StepSize
    }

    Name (LSTM,0)            // Last temperature reported

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

    Name (TLPO, Package()
    { 
      1,                           // Revision, DwordConst
      1,                           // LPOEnable, DwordConst
      0,                           // LPOStartPState, DwordConst
      1,                           // LPOStepSize, DwordConst
      1,                           // LPOPowerControlSetting, DwordConst
      2                            // LPOPerformanceControlSetting, DwordConst
    })

    // CLPO (Current Logical Processor Off lining Setting)
    // 
    // This object is utilized only by DPPM Passive policy. It is not required to implement this object for LPM.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   This object evaluates to a package that indicates the LPO Control preferences. 
    //
    Method(CLPO,,,,PkgObj)
    { 
      Store (LPOE,Index(TLPO,1)) // LPOEnable

      If(CondRefOf(\_PR.CPU0._PSS)){           // LPOStartPState
        Store(SizeOf(\_PR.CPU0._PSS()),Local1) // bounds check with _PSS
      }                                        //
      Else                                     //
      {                                        //
        Store (0,Local1)                       //
      }                                        //
      If(LLess(LPOP,Local1)){                  //
        Store (LPOP,Index(TLPO,2))             //
      }                                        //
      Else                                     //
      {                                        //
        Decrement(Local1)                      // Index of LFM entry in _PSS
        Store (Local1,Index(TLPO,2))           //
      }                                        //

      Store (LPOS,Index(TLPO,3)) // LPOStepSize
      Store (LPOW,Index(TLPO,4)) // LPOPowerControlSetting
      Store (LPER,Index(TLPO,5)) // LPOPerformanceControlSetting

      Return(TLPO)
    }

    // AEXL (Application Exclusion List)
    //
    // There is a limit of 256 strings for this package. If the list is a bigger number than the limit, it is not effective to use LPO.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   This object evaluates to a package of strings representing the application names that the DPTF processor participant 
    //   will exclude from core off lining when LPO is triggered.
    //
    Name (AEXL, Package()
    { 
      "svchost.exe", "dllhost.exe", "smss.exe", "WinSAT.exe"
    })

    // PCCC (Participant Current Control Capabilities)
    //
    // The PCCC object evaluates to a package of packages that indicates to DPTF processor participant the Icc control capabilities.
    //
    // Arguments: (0)
    //   None
    //
    // Return Value:
    //   A package of packages as described below:
    //
    //    Package()
    //    {
    //      0x80000000,       // DWordConst, Revision
    //      Package ()
    //      { 
    //        0x80000000,     // DWordConst, CurrentLimitMinimum in milli Amps
    //        0x80000000,     // DWordConst, CurrentLimitMaximum in milli Amps
    //      }
    //    }
    Method(PCCC,0,Serialized,,PkgObj)
    {
      Store (1,Index(PCCX,0))  // Revision
      Switch(ToInteger(CPNU(PTDP,0))){    // SKU check
        case(57){
          Store (43000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (95000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(47){
          Store (39000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (85000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(37){
          Store (29000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (55000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(25){
          Store (16000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (32000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(15){
          Store (14000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (32000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(11){
          Store (14000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (25000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        Default{ // UNKNOWN SKU
          Store (0xFF,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (0xFF,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
      } // End of Switch(PTDP)

      Return(PCCX)
    } // End of PCCC object

    // PCCX (Participant Current Control Capabilities temp structure)
    //
    // This is used to pass data from the PCCC object to the PDRT object.
    //
    Name (PCCX, Package()
    {
      0x80000000,       // DWordConst, Revision

      Package ()
      { 
        0x80000000,     // DWordConst, CurrentLimitMinimum
        0x80000000,     // DWordConst, CurrentLimitMaximum
      }
    }) // End of PCCC object

    // KEFF (VR efficiency Table)
    //
    // This object evaluates to a package of packages that indicates the VR efficiency factor for various processor power.
    //
    // Arguments: (0)
    //   None
    //
    // Return Value:
    //   A package of packages.
    //
    Name(KEFF, Package()
    {
      // Processor PMAX, Efficiency
      Package () {444, 0},
      Package () {463, 39},
      Package () {481, 75},
      Package () {499, 108},
      Package () {518, 139},
      Package () {536, 168},
      Package () {554, 195},
      Package () {573, 221},
      Package () {591, 244},
      Package () {609, 267},
      Package () {628, 287},
      Package () {812, 445},
      Package () {983, 551},
      Package () {1163, 621},
      Package () {1342, 673},
      Package () {1527, 710},
      Package () {1704, 742},
      Package () {1885, 767},
      Package () {2072, 785},
      Package () {2255, 802},
      Package () {6044, 897},
      Package () {11740, 924},
      Package () {17576, 926},
      Package () {23605, 919},
      Package () {29821, 909},
      Package () {36223, 898},
      Package () {42856, 886},
      Package () {49723, 873},
      Package () {56870, 858},
      Package () {64380, 842}
    })

    Name (CEUP, Package() 
    {  
      0x80000000, 
      0x80000000, 
      0x80000000, 
      0x80000000, 
      0x80000000, 
      0x80000000
    })

    // CEUC (Current Execution Unit Configuration) 
    //
    // This object evaluates to a package that indicates the Execution Unit Control preferences.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    Package {
    //      Revision       // DWordConst  - 0x1 for Sharkbay and Clovertrail platform.
    //      EUEnable       // DwordConst  - If this option is 0 then policy does not use any active core controls for graphics.
    //      TargetGfxFreq  // DwordConst  - Returns graphics frequency in MHz.
    //      MaxActiveEUs   // DwordConst  - Provides the max limit for the number of EUs that driver can request while in passive control.
    //      MinActiveEUs   // DwordConst  - Provides the min limit for the number of EUs that driver can request while in passive control.
    //      StepSize       // DwordConst  - Instructs the policy to take away EU in the specified percentage steps. E.g. if StepSize is 50% then policy would take away 1/2 of EUs at a time.
    //    }
    //
    Method(CEUC,,,,PkgObj)
    {  
      Store(0x1, Index(CEUP,0))  // Revision
      Store(ECEU, Index(CEUP,1)) // Enable/disable
      Store(TGFG, Index(CEUP,2)) // Gfx frequency
      Store(40, Index(CEUP,3))   // Max
      Store(20, Index(CEUP,4))   // Min
      Store(20, Index(CEUP,5))   // step size

      Return(CEUP)
    } // end of CEUC object
 
    // _TMP (Temperature)
    //
    // This control method returns the thermal zone's current operating temperature.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the current temperature of the thermal zone (in tenths of degrees Kelvin)
    //
//[-start-130208-IB07360234-remove]//
//     Method(_TMP)
//     {
//       // Return the temperature to the OS if EC access is enabled.
//       If(\ECON)
//       {
//         Return(\_SB.IETM.CTOK(\_SB.PCI0.LPCB.H_EC.PMAX)) 
//       }
//       Else
//       {
//         // Return a static value if EC access is disabled.
//         Return(3000)
//       }
//     }
//[-end-130208-IB07360234-remove]//

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
      Notify(B0D4, 0x91) // notify the participant of a trip point change event
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

  }// End Device(B0D4)
} // End Scope(\_SB.PCI0)
