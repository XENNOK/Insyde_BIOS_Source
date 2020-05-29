/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c) 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  LakeTiny.asl

Abstract:

  Intel Processor Power Management ACPI Code - LakeTiny Support

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
	"LakeTiny.aml",
	"SSDT",
	1,
	"PmRef",
	"LakeTiny",
	0x3000
	)
{
External(\_SB.PCI0.SAT0, DeviceObj)
External(\_SB.PCI0.SAT1, DeviceObj)
External(\PNOT, MethodObj)
External(\_PR.CPU0.GEAR)
External(MPMF)       //Bit0: LakeTiny Support Enable/Disable

Scope(\_SB.PCI0.SAT0) {
    //
    // Lake Tiny Performance Control Methods
    //
    Method(SLT1,0, Serialized) 
    {
        If (CondRefOf(\_PR.CPU0.GEAR))
        {
            Store (0x00, \_PR.CPU0.GEAR) // Select Lake Tiny CST GEAR 1
            \PNOT ()                    // OS notification for _CST evaluation
        }
	Return(0)		
    }

    Method(SLT2,0, Serialized) 
    {
        If (CondRefOf(\_PR.CPU0.GEAR))
        {
            Store (0x01, \_PR.CPU0.GEAR) // Select Lake Tiny CST GEAR 2
            \PNOT ()                    // OS notification for _CST evaluation
        }
 
	Return(0)		
    }

    Method(SLT3,0, Serialized)
    {
        If (CondRefOf(\_PR.CPU0.GEAR))
        {
            Store (0x02, \_PR.CPU0.GEAR) // Select Lake Tiny CST GEAR 3
            \PNOT ()                    // OS notification for _CST evaluation
        }
 
	Return(0)		
    } 

    Method(GLTS,0, Serialized)
    {
      Store(\_PR.CPU0.GEAR,local0)
      ShiftLeft(local0,1,local0) // Bits 1:2 is Gear
      Or(local0,0x01,local0)     // Bit 0 enable/disable
      Return(local0)
    }
}

Scope(\_SB.PCI0.SAT1){
    //
    // Lake Tiny Performance Control Methods
    //
    Method(SLT1,0, Serialized) 
    {
        If (CondRefOf(\_PR.CPU0.GEAR))
        {
            Store (0x00, \_PR.CPU0.GEAR) // Select Lake Tiny CST GEAR 1
            \PNOT ()                    // OS notification for _CST evaluation
        }
	Return(0)		
    }

    Method(SLT2,0, Serialized) 
    {
        If (CondRefOf(\_PR.CPU0.GEAR))
        {
            Store (0x01, \_PR.CPU0.GEAR) // Select Lake Tiny CST GEAR 2
            \PNOT ()                    // OS notification for _CST evaluation
        }
 
	Return(0)		
    }

    Method(SLT3,0, Serialized)
    {
        If (CondRefOf(\_PR.CPU0.GEAR))
        {
            Store (0x02, \_PR.CPU0.GEAR) // Select Lake Tiny CST GEAR 3
            \PNOT ()                    // OS notification for _CST evaluation
        }
 
	Return(0)		
    } 

    Method(GLTS,0, Serialized)
    {
      Store(\_PR.CPU0.GEAR,local0) 
      ShiftLeft(local0,1,local0) // Bits 1:2 is Gear
      And(MPMF,01,local1)
      Or(local0,local1,local0)     // Bit 0 enable/disable
      Return(local0)
    }
}

}  // End of Definition Block
