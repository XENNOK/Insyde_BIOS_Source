/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  Cpc.asl

Abstract:

  Intel ACPI Reference Code for ACPI 5.0 Collaborative Processor Performance Control (CPPC) 

--*/

DefinitionBlock (
	"Cpc.aml",
	"SSDT",
	1,
	"Cpc_Tabl",
	"Cpc_Tabl",
	0x1000 )
{

External(\_PR.CPU0, ProcessorObj)
External(\_PR.CPU1, ProcessorObj)
External(\_PR.CPU2, ProcessorObj)
External(\_PR.CPU3, ProcessorObj)
External(\_PR.CPU4, ProcessorObj)
External(\_PR.CPU5, ProcessorObj)
External(\_PR.CPU6, ProcessorObj)
External(\_PR.CPU7, ProcessorObj)

// _CPC (Continuous Performance Control)
//	_CPC is a per-processor ACPI object that declares an interface for OSPM to 
//	transition the processor into a performance state based on a continuous range 
//	of allowable values.  Each CPPC register is described in a Generic Register
//	Descriptor format and maps to an unique PCC shared memory 
//	location.  For a complete description of _CPC object, refer to ACPI 5.0
//	specification, section 8.4.5.1.
// Arguments: (0)
// 	None
// Return Value:
//	A Package of elements in the following format
//
//	Package
//	{
//		NumEntries,				// Integer
//		Revision,				// Integer
//		HighestPerformance,			// Generic Register Descriptor
//		NominalPerformance,			// Generic Register Descriptor
//		LowestNonlinearPerformance,		// Generic Register Descriptor
//		LowestPerformance,			// Generic Register Descriptor
//		GuaranteedPerformanceRegister,	// Generic Register Descriptor
//		DesiredPerformanceRegister,		// Generic Register Descriptor
//		MinimumPerformanceRegister,		// Generic Register Descriptor
//		MaximumPerformanceRegister,		// Generic Register Descriptor
//		PerformanceReductionToleranceRegister,// Generic Register Descriptor
//		TimeWindowRegister,			// Generic Register Descriptor
//		CounterWraparoundTime,		// Generic Register Descriptor
//		NominalCounterRegister,		// Generic Register Descriptor
//		DeliveredCounterRegister,		// Generic Register Descriptor
//		PerformanceLimitedRegister,		// Generic Register Descriptor
//		EnableRegister				// Generic Register Descriptor
//	}

Scope(\_PR.CPU0)
{
  Name(_CPC, Package()
  {
    17, // Number of entries
    1,  // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate() {Register(PCC, 32, 0, 0x0000, 0)}, // HighestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0004, 0)}, // NominalPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0008, 0)}, // LowestNonlinearPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x000C, 0)}, // LowestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0010, 0)}, // GuaranteedPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0014, 0)}, // DesiredPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0018, 0)}, // MinimumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x001C, 0)}, // MaximumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0020, 0)}, // PerformanceReductionToleranceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0024, 0)}, // TimeWindowRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0028, 0)}, // CounterWraparoundTime
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // NominalCounterRegister
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // DeliveredCounterRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x003C, 0)}, // PerformanceLimitedRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0040, 0)}  // EnableRegister
  })
}// end Scope(\_PR.CPU0)

Scope(\_PR.CPU1)
{
  Name(_CPC, Package()
  {
    17, // Number of entries
    1,  // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate() {Register(PCC, 32, 0, 0x0064, 0)}, // HighestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0068, 0)}, // NominalPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x006C, 0)}, // LowestNonlinearPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0070, 0)}, // LowestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0074, 0)}, // GuaranteedPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0078, 0)}, // DesiredPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x007C, 0)}, // MinimumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0080, 0)}, // MaximumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0084, 0)}, // PerformanceReductionToleranceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0088, 0)}, // TimeWindowRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x008C, 0)}, // CounterWraparoundTime
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // NominalCounterRegister
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // DeliveredCounterRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x00A0, 0)}, // PerformanceLimitedRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x00A4, 0)}  // EnableRegister
  })
}// end Scope(\_PR.CPU1)

Scope(\_PR.CPU2)
{
  Name(_CPC, Package()
  {
    17, // Number of entries
    1,  // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate() {Register(PCC, 32, 0, 0x00C8, 0)}, // HighestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x00CC, 0)}, // NominalPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x00D0, 0)}, // LowestNonlinearPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x00D4, 0)}, // LowestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x00D8, 0)}, // GuaranteedPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x00DC, 0)}, // DesiredPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x00E0, 0)}, // MinimumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x00E4, 0)}, // MaximumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x00E8, 0)}, // PerformanceReductionToleranceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x00EC, 0)}, // TimeWindowRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x00F0, 0)}, // CounterWraparoundTime
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // NominalCounterRegister
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // DeliveredCounterRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0104, 0)}, // PerformanceLimitedRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0108, 0)}  // EnableRegister
  })
}// end Scope(\_PR.CPU2)

Scope(\_PR.CPU3)
{
  Name(_CPC, Package()
  {
    17, // Number of entries
    1,  // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate() {Register(PCC, 32, 0, 0x012C, 0)}, // HighestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0130, 0)}, // NominalPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0134, 0)}, // LowestNonlinearPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0138, 0)}, // LowestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x013C, 0)}, // GuaranteedPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0140, 0)}, // DesiredPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0144, 0)}, // MinimumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0148, 0)}, // MaximumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x014C, 0)}, // PerformanceReductionToleranceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0150, 0)}, // TimeWindowRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0154, 0)}, // CounterWraparoundTime
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // NominalCounterRegister
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // DeliveredCounterRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0168, 0)}, // PerformanceLimitedRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x016C, 0)}  // EnableRegister
  })
}// end Scope(\_PR.CPU3)

Scope(\_PR.CPU4)
{
  Name(_CPC, Package()
  {
    17, // Number of entries
    1,  // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate() {Register(PCC, 32, 0, 0x0190, 0)}, // HighestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0194, 0)}, // NominalPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0198, 0)}, // LowestNonlinearPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x019C, 0)}, // LowestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x01A0, 0)}, // GuaranteedPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x01A4, 0)}, // DesiredPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x01A8, 0)}, // MinimumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x01AC, 0)}, // MaximumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x01B0, 0)}, // PerformanceReductionToleranceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x01B4, 0)}, // TimeWindowRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x01B8, 0)}, // CounterWraparoundTime
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // NominalCounterRegister
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // DeliveredCounterRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x01CC, 0)}, // PerformanceLimitedRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x01D0, 0)}  // EnableRegister
  })
}// end Scope(\_PR.CPU4)

Scope(\_PR.CPU5)
{
  Name(_CPC, Package()
  {
    17, // Number of entries
    1,  // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate() {Register(PCC, 32, 0, 0x01F4, 0)}, // HighestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x01F8, 0)}, // NominalPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x01FC, 0)}, // LowestNonlinearPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0200, 0)}, // LowestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0204, 0)}, // GuaranteedPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0208, 0)}, // DesiredPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x020C, 0)}, // MinimumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0210, 0)}, // MaximumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0214, 0)}, // PerformanceReductionToleranceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0218, 0)}, // TimeWindowRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x021C, 0)}, // CounterWraparoundTime
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // NominalCounterRegister
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // DeliveredCounterRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0230, 0)}, // PerformanceLimitedRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0234, 0)}  // EnableRegister
  })
}// end Scope(\_PR.CPU5)

Scope(\_PR.CPU6)
{
  Name(_CPC, Package()
  {
    17, // Number of entries
    1,  // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate() {Register(PCC, 32, 0, 0x0258, 0)}, // HighestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x025C, 0)}, // NominalPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0260, 0)}, // LowestNonlinearPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0264, 0)}, // LowestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x0268, 0)}, // GuaranteedPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x026C, 0)}, // DesiredPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0270, 0)}, // MinimumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0274, 0)}, // MaximumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0278, 0)}, // PerformanceReductionToleranceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x027C, 0)}, // TimeWindowRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0280, 0)}, // CounterWraparoundTime
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // NominalCounterRegister
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // DeliveredCounterRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0294, 0)}, // PerformanceLimitedRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x0298, 0)} // EnableRegister
  })
}// end Scope(\_PR.CPU6)

Scope(\_PR.CPU7)
{
  Name(_CPC, Package()
  {
    17, // Number of entries
    1,  // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate() {Register(PCC, 32, 0, 0x02BC, 0)}, // HighestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x02C0, 0)}, // NominalPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x02C4, 0)}, // LowestNonlinearPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x02C8, 0)}, // LowestPerformance
    ResourceTemplate() {Register(PCC, 32, 0, 0x02CC, 0)}, // GuaranteedPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x02D0, 0)}, // DesiredPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x02D4, 0)}, // MinimumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x02D8, 0)}, // MaximumPerformanceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x02DC, 0)}, // PerformanceReductionToleranceRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x02E0, 0)}, // TimeWindowRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x02E4, 0)}, // CounterWraparoundTime
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // NominalCounterRegister
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // DeliveredCounterRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x02F8, 0)}, // PerformanceLimitedRegister
    ResourceTemplate() {Register(PCC, 32, 0, 0x02FC, 0)}  // EnableRegister
  })
}// end Scope(\_PR.CPU7)

}// end of definition block
