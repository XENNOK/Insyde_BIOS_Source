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

DefinitionBlock (
  "CPUPM.aml",
  "SSDT",
  0x01,
  "PmRef",
  "CpuPm",
  0x3000
  )
{
  External(\_PR.CPU0, DeviceObj)
  External(\_PR.CPU1, DeviceObj)
  External(\_PR.CPU2, DeviceObj)
  External(\_PR.CPU3, DeviceObj)
  External(\_PR.CPU4, DeviceObj)
  External(\_PR.CPU5, DeviceObj)
  External(\_PR.CPU6, DeviceObj)
  External(\_PR.CPU7, DeviceObj)
//[-start-130328-IB10310017-modify]//
#if SPS_ME_SUPPORT
  External(\PSLM, IntObj)
#else
  External(\_PR.CPU0._PPC, IntObj)
#endif
//[-end-130328-IB10310017-modify]//
  External(SMIF)

Scope(\)
{

  // Package of pointers to SSDT's
  //
  // First column is SSDT name, used for debug only.
  // (First column must be EXACTLY eight characters.)
  // Second column is physical address.
  // Third column is table length.
  //
  // IF modifying this file, see warnings listed in ppminit.asm.
  //
  Name(SSDT,Package()
  {
    "CPU0IST ", 0x80000000, 0x80000000,
    "APIST   ", 0x80000000, 0x80000000,
    "CPU0CST ", 0x80000000, 0x80000000,
    "APCST   ", 0x80000000, 0x80000000
  })

  Name(\PDC0,0x80000000)  // CPU0 _PDC Flags.
  Name(\PDC1,0x80000000)  // CPU1 _PDC Flags.
  Name(\PDC2,0x80000000)  // CPU2 _PDC Flags.
  Name(\PDC3,0x80000000)  // CPU3 _PDC Flags.
  Name(\PDC4,0x80000000)  // CPU4 _PDC Flags.
  Name(\PDC5,0x80000000)  // CPU5 _PDC Flags.
  Name(\PDC6,0x80000000)  // CPU6 _PDC Flags.
  Name(\PDC7,0x80000000)  // CPU7 _PDC Flags.

  Name(\SDTL,0x00)        // Loaded SSDT Flags.
}

Scope(\_PR)
{
  //
  // Define a Processor scope ACPI PPM GlobalNvs NVS Region
  //
  OperationRegion(PPMT,SystemMemory,0xFFFF0000,0xAA55)
  Field(PPMT,AnyAcc,Lock,Preserve)
  {
    PGRV,       8,      // (0) PPM GlobalNvs Revision
    CFGD,      32,      // CFGD - PpmFlags
    Offset(6),          // (5) Reserved
    //
    // Thermal Configuration Values
    //
    ACRT,    8,        // (6) Auto Critical Trip Point
    APSV,    8,        // (7) Auto Passive Trip Point
    AAC0,    8,        // (8) Auto Active Trip Point
    CPID,    32,       // (9) CPUID
    //
    // ConfigTDP Value
    //
    CPPC,    8,        // (13) Boot Mode vlues for _PPC
    //
    // ConfigTDP Level settngs
    //
    CCTP,    8,        // (14) Custom ConfigTdp Enabled/Disabled
    CLVL,    8,        // (15) ConfigTdp Number Of Levels
    CBMI,    8,        // (16) CTDP Boot Mode Index
    PL10,    16,       // (17) CTDP Level 0 Power Limit1
    PL20,    16,       // (19) CTDP Level 0 Power Limit2
    PLW0,    8,        // (21) CTDP Level 0 Power Limit1 Time Window
    CTC0,    8,        // (22) CTDP Level 0 CTC
    TAR0,    8,        // (23) CTDP Level 0 TAR
    PPC0,    8,        // (24) CTDP Level 0 PPC
    PL11,    16,       // (25) CTDP Level 1 Power Limit1
    PL21,    16,       // (27) CTDP Level 1 Power Limit2
    PLW1,    8,        // (29) CTDP Level 1 Power Limit1 Time Window
    CTC1,    8,        // (30) CTDP Level 1 CTC
    TAR1,    8,        // (31) CTDP Level 1 TAR
    PPC1,    8,        // (32) CTDP Level 1 PPC
    PL12,    16,       // (33) CTDP Level 2 Power Limit1
    PL22,    16,       // (35) CTDP Level 2 Power Limit2
    PLW2,    8,        // (37) CTDP Level 2 Power Limit1 Time Window
    CTC2,    8,        // (38) CTDP Level 2 CTC
    TAR2,    8,        // (39) CTDP Level 2 TAR
    PPC2,    8,         // (40) CTDP Level 2 PPC
    //
    // Mwait Hints and Latency values for C3/C6/C7/C7S
    //
    C3MW,    8,        // (41) Mwait Hint value for C3
    C6MW,    8,        // (42) Mwait Hint value for C6
    C7MW,    8,        // (43) Mwait Hint value for C7/C7s
    CDMW,    8,        // (44) Mwait Hint value for C8/C9/C10
    C3LT,   16,        // (45-46) Latency value for C3
    C6LT,   16,        // (47-48) Latency Value for C6
    C7LT,   16,        // (49-50) Latency Value for C7/C7S
    CDLT,   16,        // (51-52) Latency Value for C8/C9/C10
    CDLV,   16,        // (53-54) IO LVL value for C8/C9/C10
    CDPW,   16,        // (55-56) Power value for C8/C9/C10
    MPMF,   8          // (57) MiscPowerManagementFlags
  }
}

Scope(\_PR.CPU0)
{
  //
  // Define handles for opregions (used by load.)
  //
  Name(HI0,0)    // Handle to CPU0IST
  Name(HC0,0)    // Handle to CPU0CST

  Method(_PDC,1)
  {
    //
    // Update the _PPC value
    //
    if(CondRefOf(\_PR.CPU0._PPC)) {
//[-start-130328-IB10310017-add]//
#if SPS_ME_SUPPORT
      Store (\PSLM, CPPC)
#else
      Store (CPPC, \_PR.CPU0._PPC)
#endif
//[-end-130328-IB10310017-add]//
    }
    //
    // Check and extract the _PDC information.
    //
    Store(CPDC(Arg0), Local0)
    //
    // Save the capability information and load tables as needed.
    //
    GCAP(Local0)
    //
    // Return status.
    //
    Return (Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Check and extract the _OSC information.
    //
    Store(COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    //
    // Save the capability information and load tables as needed.
    //
    GCAP(Local0)
    //
    // Return status.
    //
    Return (Local0)
  }

  //
  // Implement a generic Method to check _PDC information which may be called
  // by any of the processor scopes.  (The use of _PDC is deprecated in ACPI 3.
  // in favor of _OSC. However, for backwards compatibility, _PDC may be
  // implemented using _OSC as follows:)
  //
  Method(CPDC,1)
  {
    CreateDwordField (Arg0, 0, REVS)
    CreateDwordField (Arg0, 4, SIZE)

    //
    // Local0 = Number of bytes for Arg0
    //
    Store (SizeOf (Arg0), Local0)

    //
    // Local1 = Number of Capabilities bytes in Arg0
    //
    Store (Subtract (Local0, 8), Local1)

    //
    // TEMP = Temporary field holding Capability DWORDs
    //
    CreateField (Arg0, 64, Multiply (Local1, 8), TEMP)

    //
    // Create the Status (STAT) buffer with the first DWORD = 0
    // This is required as per ACPI 3.0 Spec which says the
    // first DWORD is used to return errors defined by _OSC.
    //
    Name (STS0, Buffer () {0x00, 0x00, 0x00, 0x00})

    //
    // Concatenate the _PDC capabilities bytes to the STS0 Buffer
    // and store them in a local variable for calling OSC
    //
    Concatenate (STS0, TEMP, Local2)

    Return(COSC (ToUUID("4077A616-290C-47BE-9EBD-D87058713953"), REVS, SIZE, Local2))
  }

  //
  // Implement a generic Method to check _OSC information which may be called
  // by any of the processor scopes.
  //
  Method(COSC, 4)
  {
    //
    // Point to Status DWORD in the Arg3 buffer (STATUS)
    //
    CreateDWordField(Arg3, 0, STS0)
    //
    // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg3, 4, CAP0)

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
    // Note:  The comparison method used is necessary due to
    // limitations of certain OSes which cannot perform direct
    // buffer comparisons.
    //
    // Create a set of "Input" UUID fields.
    //
    CreateDwordField(Arg0, 0x0, IID0)
    CreateDwordField(Arg0, 0x4, IID1)
    CreateDwordField(Arg0, 0x8, IID2)
    CreateDwordField(Arg0, 0xC, IID3)
    //
    // Create a set of "Expected" UUID fields.
    //
    Name(UID0, ToUUID("4077A616-290C-47BE-9EBD-D87058713953"))
    CreateDwordField(UID0, 0x0, EID0)
    CreateDwordField(UID0, 0x4, EID1)
    CreateDwordField(UID0, 0x8, EID2)
    CreateDwordField(UID0, 0xC, EID3)
    //
    // Verify the input UUID matches the expected UUID.
    //
    If(LNot(LAnd(LAnd(LEqual(IID0, EID0),LEqual(IID1, EID1)),LAnd(LEqual(IID2, EID2),LEqual(IID3, EID3)))))
    {
      //
      // Return Unrecognized UUID _OSC Failure
      //
      Store (0x6, STS0)
      Return (Arg3)
    }

    If(LNot(LEqual(Arg1,1)))
    {
      //
      // Return Unsupported Revision _OSC Failure
      //
      Store (0xA, STS0)
      Return (Arg3)
    }

    Return (Arg3)
  }

  //
  // Get the capability information and load appropriate tables as needed.
  //
  Method(GCAP, 1)
  {
    //
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    //
    CreateDWordField(Arg0, 0, STS0)
    //
    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg0, 4, CAP0)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(STS0,0x6),LEqual(STS0,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(STS0, 1))
    {
      And(CAP0, 0xBFF, CAP0)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PDC0, 0x7FFFFFFF), CAP0, PDC0)

    //
    // Check IF the CST SSDTs should be loaded.
    //   CFGD[5:1] = C7, C6, C3, C1E, C1 Capable/Enabled
    If(And(CFGD, PPM_C_STATES))
    {
      //
      // Load the CST SSDTs if:
      //   (1) CMP capable/enabled
      //   (2) Driver supports multi-processor configurations
      //   (3) CPU0 CST ISDT is not already loaded
      //
      //   CFGD[9] = Two or more cores enabled
      //   PDCx[3]  = OS supports C1 and P-states in MP systems
      //   PDCx[4]  = OS supports ind. C2/C3 in MP systems
      //   SDTL[1]  = CPU0 CST SSDT Loaded
      //
      If(LAnd(LAnd(And(CFGD, PPM_CMP),And(PDC0,0x0018)),LNot(And(SDTL,0x02))))
      {
        //
        // Flag the CST SSDT as loaded for CPU0
        //
        Or(SDTL, 0x02, SDTL)

        OperationRegion(CST0,SystemMemory,DeRefOf(Index(SSDT,7)),DeRefOf(Index(SSDT,8)))
        Load(CST0, HC0)  // Dynamically load the CPU0CST SSDT
      }
    }

    Return ()
  }
}


Scope(\_PR.CPU1)
{
  //
  // Define handles for opregions (used by load.)
  //
  Name(HI1,0)    // Handle to APIST
  Name(HC1,0)    // Handle to APCST

  Method(_PDC,1)
  {
    //
    // Refer to \_PR.CPU0._PDC for description.
    //
    Store(\_PR.CPU0.CPDC(Arg0), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Refer to \_PR.CPU0._OSC for description.
    //
    Store(\_PR.CPU0.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  //
  // Get the capability information and load appropriate tables as needed.
  //
  Method(GCAP, 1)
  {
    //
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    //
    CreateDWordField(Arg0, 0, STS1)
    //
    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg0, 4, CAP1)
    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(STS1,0x6),LEqual(STS1,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(STS1, 1)) 
    {
      And(CAP1, 0xBFF, CAP1)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PDC1, 0x7FFFFFFF), CAP1, PDC1)

    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PDC1, 0x0009), 0x0009))
    {
      APPT()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PDC1,0x0018))
    {
      APCT()
    }

    Store (PDC1, PDC0)

    Return()
  }

  //
  // Dynamically load the CST SSDTs if:
  //   (1) C-States are enabled
  //   (2) SSDT is not already loaded
  //
  //   CFGD[5:1] = Basic C-States supported (C1, C1E, C3, C6, C7)
  //   SDTL[5]   = AP CST SSDT Loaded
  //
  Method(APCT,0)
  {
    If(LAnd(And(CFGD,PPM_C_STATES),LNot(And(SDTL,0x20))))
    {
      //
      // Flag the CST SSDT as loaded for the AP's
      //
      Or(SDTL, 0x20, SDTL)
      //
      // Dynamically load the APCST SSDT
      //
      OperationRegion(CST1,SystemMemory,DeRefOf(Index(SSDT,10)),DeRefOf(Index(SSDT,11)))
      Load(CST1, HC1)  
    }
  }

  //
  // Dynamically load the IST SSDTs if:
  //   (1) If GV3 capable and enabled
  //   (2) SSDT is not already loaded
  //
  //   CFGD[0] = GV3 Capable/Enabled
  //   SDTL[4] = AP IST SSDT Loaded
  //
  Method(APPT,0)
  {
    If(LAnd(And(CFGD,PPM_EIST),LNot(And(SDTL,0x10))))
    {
      //
      // Flag the IST SSDT as loaded for CPU0
      //
      Or(SDTL, 0x10, SDTL)

      OperationRegion(IST1,SystemMemory,DeRefOf(Index(SSDT,4)),DeRefOf(Index(SSDT,5)))
      Load(IST1, HI1)  // Dynamically load the CPU1IST SSDT
    }
  }
}  // End CPU1


Scope(\_PR.CPU2)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_PR.CPU0.CPDC(Arg0), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_PR.CPU0.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, STS2)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CAP2)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(STS2,0x6),LEqual(STS2,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(STS2, 1)) 
    {
      And(CAP2, 0xBFF, CAP2)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PDC2, 0x7FFFFFFF), CAP2, PDC2)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PDC2, 0x0009), 0x0009))
    {
      \_PR.CPU1.APPT()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PDC2,0x0018))
    {
      \_PR.CPU1.APCT()
    }

    Store (PDC2, PDC0)
    Return()
  }
}  // End CPU2

Scope(\_PR.CPU3)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_PR.CPU0.CPDC(Arg0), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_PR.CPU0.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, STS3)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CAP3)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(STS3,0x6),LEqual(STS3,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(STS3, 1)) 
    {
      And(CAP3, 0xBFF, CAP3)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PDC3, 0x7FFFFFFF), CAP3, PDC3)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PDC3, 0x0009), 0x0009))
    {
      \_PR.CPU1.APPT()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PDC3,0x0018))
    {
      \_PR.CPU1.APCT()
    }

    Store (PDC3, PDC0)
    Return()
  }
} // End CPU3

Scope(\_PR.CPU4)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_PR.CPU0.CPDC(Arg0), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_PR.CPU0.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, STS4)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CAP4)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(STS4,0x6),LEqual(STS4,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(STS4, 1)) 
    {
      And(CAP4, 0xBFF, CAP4)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PDC4, 0x7FFFFFFF), CAP4, PDC4)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PDC4, 0x0009), 0x0009))
    {
      \_PR.CPU1.APPT()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PDC4,0x0018))
    {
      \_PR.CPU1.APCT()
    }

    Store (PDC4, PDC0)
    Return()
  }
} // End CPU4

Scope(\_PR.CPU5)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_PR.CPU0.CPDC(Arg0), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_PR.CPU0.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, STS5)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CAP5)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(STS5,0x6),LEqual(STS5,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(STS5, 1)) 
    {
      And(CAP5, 0xBFF, CAP5)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PDC5, 0x7FFFFFFF), CAP5, PDC5)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PDC5, 0x0009), 0x0009))
    {
      \_PR.CPU1.APPT()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PDC5,0x0018))
    {
      \_PR.CPU1.APCT()
    }

    Store (PDC5, PDC0)
    Return()
  }
} // End CPU5

Scope(\_PR.CPU6)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_PR.CPU0.CPDC(Arg0), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_PR.CPU0.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, STS6)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CAP6)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(STS6,0x6),LEqual(STS6,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(STS6, 1)) 
    {
      And(CAP6, 0xBFF, CAP6)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PDC6, 0x7FFFFFFF), CAP6, PDC6)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PDC6, 0x0009), 0x0009))
    {
      \_PR.CPU1.APPT()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PDC6,0x0018))
    {
      \_PR.CPU1.APCT()
    }

    Store (PDC6, PDC0)
    Return()
  }
} // End CPU6

Scope(\_PR.CPU7)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_PR.CPU0.CPDC(Arg0), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_PR.CPU0.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, STS7)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CAP7)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(STS7,0x6),LEqual(STS7,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(STS7, 1)) 
    {
      And(CAP7, 0xBFF, CAP7)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PDC7, 0x7FFFFFFF), CAP7, PDC7)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PDC7, 0x0009), 0x0009))
    {
      \_PR.CPU1.APPT()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PDCx[3]  = OS supports C1 and P-states in MP systems
    //   PDCx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PDC7,0x0018))
    {
      \_PR.CPU1.APCT()
    }

    Store (PDC7, PDC0)
    Return()
  }
} // End CPU7
} // End of Definition Block
