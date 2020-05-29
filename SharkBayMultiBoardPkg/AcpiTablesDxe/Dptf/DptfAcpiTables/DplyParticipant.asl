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

  DplyParticipant.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/
Scope(\_SB.PCI0)
{

  Device(DPLY)  // Display participant
  {
    Name(_HID, EISAID("INT3406"))
    Name(_UID, 0)

    Name(BDLI, 32)  // Brightness depth limit index corresponding to 30%
    Name(BDHI, 82) // Brightness peformance/power ceiling limit index corresponding to 80%


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
      If (LEqual(DISE,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // DDDL ( Dptf Display Depth Limit)
    //
    // The DDDL object indicates dynamically a lower limit on the brightness control levels currently supported by the platform 
    // for the participant. Value returned indicates a Power/Percentage value that is in the _BCL brightness list.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Index in Brightness level list
    //
    Method(DDDL, 0)
    {
      Return(BDLI)
    }

    // DDPC ( Dptf Display Power/performance Control)
    //
    // The DDPC object indicates dynamically a higher limit (ceiling) on the brightness control levels currently supported by 
    // the platform for the participant.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Index in Brightness level list
    //
    Method(DDPC, 0)
    {
      Return(BDHI)
    }

    // _BCL (Query List of Brightness Control Levels Supported)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Integers representing the supported brightness levels. 
    //   Each integer has 8 bits of significant data.
    //    List of supported brightness levels in the following sequence:
    //     Level when machine has full power.
    //     Level when machine is on batteries.
    //     Other supported levels.
    //     Package(){80, 50, 0, 1, 2, 3, 4, 5, ..., 96, 97, 98, 99, 100}
    //
    Method(_BCL,,,,PkgObj)
    {
      If(CondRefOf(\_SB.PCI0.GFX0.DD1F._BCL))
      {
        Return(\_SB.PCI0.GFX0.DD1F._BCL())
      } Else {
        Return(Package(){0})
      }
    }

    // _BCM (Set the Brightness Level)
    //
    // Arguments: (1)
    //   Arg0 - An Integer containing the new brightness level
    // Return Value:
    //   None
    //
    Method(_BCM,1)
    {
      If(CondRefOf(\_SB.PCI0.GFX0.DD1F._BCM))
      {
        \_SB.PCI0.GFX0.DD1F._BCM(Arg0)
      }
    }

    // _BQC (Brightness Query Current level)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the current brightness level (must be one of the values returned from the _BCL method)
    //
    Method(_BQC,0)
    {
      If(CondRefOf(\_SB.PCI0.GFX0.DD1F._BQC))
      {
        Return(\_SB.PCI0.GFX0.DD1F._BQC())
      } Else {
        Return(0x00)
      }
    }

    // _DCS (Return the Status of Output Device)
    //
    //Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the device status (32 bits)
    //   Bit Definitions
    //   [0] - Output connector exists in the system now
    //   [1] - Output is activated
    //   [2] - Output is ready to switch
    //   [3] - Output is not defective (it is functioning properly)
    //   [4] - Device is attached (this is optional)
    //   [31:5] Reserved (must be zero)
    //
    Method(_DCS,0)
    {
      If(CondRefOf(\_SB.PCI0.GFX0.DD1F._DCS))
      {
        Return(\_SB.PCI0.GFX0.DD1F._DCS())
      } Else {
        Return(0x00)
      }
    }

  } // End DPLY Device
}// end Scope(\_SB.PCI0)
