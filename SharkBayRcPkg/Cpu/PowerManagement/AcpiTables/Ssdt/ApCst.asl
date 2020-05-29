/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2011 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  ApCst.asl

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
	"APCST.aml",
	"SSDT",
	1,
	"PmRef",
	"ApCst",
	0x3000
	)
{
External(\_PR.CPU1, DeviceObj)
External(\_PR.CPU2, DeviceObj)
External(\_PR.CPU3, DeviceObj)
External(\_PR.CPU4, DeviceObj)
External(\_PR.CPU5, DeviceObj)
External(\_PR.CPU6, DeviceObj)
External(\_PR.CPU7, DeviceObj)
External(\_PR.CPU0._CST)

	Scope(\_PR.CPU1)
	{
		Method(_CST,0)
		{
			//
			// Return P0's _CST object.
			//
			Return(\_PR.CPU0._CST)
		}
	}

	Scope(\_PR.CPU2)
	{
		Method(_CST,0)
		{
			//
			// Return P0's _CST object.
			//
			Return(\_PR.CPU0._CST)
		}
	}

	Scope(\_PR.CPU3)
	{
		Method(_CST,0)
		{
			//
			// Return P0's _CST object.
			//
			Return(\_PR.CPU0._CST)
		}
	}

	Scope(\_PR.CPU4)
	{
		Method(_CST,0)
		{
			//
			// Return P0's _CST object.
			//
			Return(\_PR.CPU0._CST)
		}
	}

	Scope(\_PR.CPU5)
	{
		Method(_CST,0)
		{
			//
			// Return P0's _CST object.
			//
			Return(\_PR.CPU0._CST)
		}
	}

	Scope(\_PR.CPU6)
	{
		Method(_CST,0)
		{
			//
			// Return P0's _CST object.
			//
			Return(\_PR.CPU0._CST)
		}
	}

	Scope(\_PR.CPU7)
	{
		Method(_CST,0)
		{
			//
			// Return P0's _CST object.
			//
			Return(\_PR.CPU0._CST)
		}
	}

}  // End of Definition Block
