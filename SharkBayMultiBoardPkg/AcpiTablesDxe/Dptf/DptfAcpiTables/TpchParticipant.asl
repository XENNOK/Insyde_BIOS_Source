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

  TpchParticipant.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/


Scope(\_SB.PCI0)
{
  //
  // This device is currently used by DPPM policies only. 
  // Refer to the specific technology BIOS specification.
  //
  Device (TPCH)
  {
    Name (_ADR,0x001F0006)		// Device 31, Function 6
    Name (LSTM,0)            // Last temperature reported

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
     If (LEqual(PCHD,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    OperationRegion(PCHT,PCI_Config,0x10,0x08) // PCH Thermal BAR
    Field(PCHT,DWordAcc,NoLock,Preserve)
    {
	Offset(0x0),		// EPBAR (Offset 10h)
	STYP,	1,		// Space type
	ADRG,	2,		// Address Range
	PREF,	1,		// Prefetchable
	, 	8,		// hardwired to 0
	TBAR,	52		// Thermal BAR 
    }

    // Thermal Sensor Hysteresis, 3 degree Centigrade
    Name(GTSH, 30)

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
      Notify(TPCH, 0x91) // notify the participant of a trip point change event
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

  } // End Device(TPCH)
} // End Scope(\_SB.PCI0)
