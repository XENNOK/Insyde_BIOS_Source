/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Ivy Bridge          *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved   *;
;*    This software and associated documentation (if any) is furnished    *;
;*    under a license and may only be used or copied in accordance        *;
;*    with the terms of the license. Except as permitted by such          *;
;*    license, no part of this software or documentation may be           *;
;*    reproduced, stored in a retrieval system, or transmitted in any     *;
;*    form or by any means without the express written consent of         *;
;*    Intel Corporation.                                                  *;
;*                                                                        *;
;*                                                                        *;
;**************************************************************************/
/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
--*/

DefinitionBlock (
    "SensorHubApp.aml",
    "SSDT",
    0x01, // SHAD revision.
        // A Revision field value greater than or equal to 2 signifies that integers 
        // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL", // OEM ID (6 byte string)
    "sensrhub", // OEM table ID  (8 byte string)
    0x0 // OEM version of DSDT table (4 byte Integer)
    )

// BEGIN OF ASL SCOPE

{
  External(\GPBS)
  External(\SDS0)
  External(\USBH)
  External(\_SB.RDGP, MethodObj)
  External(\_SB.WTGP, MethodObj)
  External(\_SB.PCI0.I2C0.SHUB, DeviceObj) // Sensor hub
  External(\_SB.PCI0.I2C0.DFUD, DeviceObj) // DFU

  Scope(\)
  {
    Device(SHAD)                  // Sensor Hub Application Device.
    {
      Name(_HID, EISAID("INT33D0"))
      Name(_CID, EISAID("PNP0C02"))

      Method(_STA, 0,Serialized)
      {
        If(LOr(And(SDS0,1), And(USBH,1)))
        {
          Return(0x000F)      // Sensor Hub Enabled, Show it
        }
        Return(0x00)          // Sensor Hub Disabled, Hide it
      }

      //
      // _DSM : Device Specific Method supporting USB Sideband Deferring function
      //
      // Arg0: UUID Unique function identifier
      // Arg1: Integer Revision Level
      // Arg2: Integer Function Index
      // Arg3: Package Parameters
      //
      Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
      {

        // Define the Local Variables used throughout the method.

        Name(PGCE, 0) // Power Gate Control Enable.
        Name(PGCD, 0) // Power Gate Control Duration.
        Name(DFUE, 0) // DFU Enable.
        Name(DFUD, 0) // DFU Duration.
        Name(OLDV, 0) // Old value of both Power Gate and DFU GPIO.
        Name(PGCV, 0) // Power Gate Control Value
        Name(DFUV, 0) // DFU Control Value

        // Compare passed in UUID to supported UUID.

        If (LEqual(Arg0, ToUUID ("03C868D5-563F-42A8-9F57-9A18D949B7CB")))  
        {

          If (LEqual(1,ToInteger(Arg1)))        // Revision 1.
          {
            Switch (ToInteger(Arg2))            // Switch to Function Index.
            {
              //
              // Function 0, Query - return supported functions BitIndex.
              //

              Case (0)
              {
                Return (Buffer() {0x0F})
              }   

              //
              // Function 1, Power Gate Control - Pass in 2 value package via Arg3
              //

              Case (1)
              {

                Store (DeRefOf (Index (Arg3, 0)), PGCE)
                Store (DeRefOf (Index (Arg3, 1)), PGCD)

                Store(\_SB.RDGP(46), OLDV)           // Save off GPIO46 = PWRGATE#
                \_SB.WTGP(46, PGCE)                  // Set value to GPIO46 = PWRGATE#

                If(LGreater(PGCD, 0))                // test duration value
                { 
                  Sleep(PGCD)
                  \_SB.WTGP(46, OLDV)                // Restore GPIO46 = PWRGATE#
                }  

                If (LEqual(\_SB.RDGP(46), 0x01))     // check for powered on (1 = on)
                {
                  Sleep(150)                         // Delay so sensor hub has time to init.
                  If (LEqual(\_SB.RDGP(44), 0x01)) { // check mode, Sensor or DFU (1 = Sensor)
                    // Powering up in sensor hub mode
                    Notify(\_SB.PCI0.I2C0.SHUB, 1)   // Check the sensor hub status
                  } Else {
                    // Powering up in DFU mode
                    Notify(\_SB.PCI0.I2C0.DFUD, 1)   // Check the DFU device status
                  }
                }

                Return (0)
              } // End Case (1)
          
              //
              // Function2, DFU Control - Pass in 2 value package via Arg3
              //

              Case (2)
              {

                Store (DeRefOf (Index (Arg3, 0)), DFUE)
                Store (DeRefOf (Index (Arg3, 1)), DFUD)

                Store(\_SB.RDGP(44), OLDV)          // Save off GPIO44 = DFU_ENA#
                \_SB.WTGP(44, DFUE)                 // Set Value to GPIO44 = DFU_ENA#

                If (LGreater(DFUD, 0))              // Test duration value
                { 
                    Sleep(DFUD)                     // Delay for passed in duration.
                    \_SB.WTGP(44, OLDV)             // Restore GPIO44 = DFU_ENA#
                } 

                Return (0) 
              } // End Case 2...

              //
              // Function 3, Query Status - Return the current status of GPIO signals.
              //

              Case (3)
              {
                Store(\_SB.RDGP(44), DFUV)
                Store(\_SB.RDGP(46), PGCV)
                Return(Package(){PGCV, DFUV})
              } // End Case 3

            } // End Function Index...
            Return (0)
          }  // End Revision check...
          Return (0)
        }  // End UUID check...
        Return (0)
      }  // End _DSM Method...
    }  // End Device....
  }  // End Scope...
}  // End DefinitionBlock Termlist