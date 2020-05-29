/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the SandyBridge         *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  2013 Intel Corporation. All rights reserved          *;
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
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

        //--------------------
        //  Intel Sensor Solution
        //--------------------
          Name (_ADR, Zero)
          Name (_UID, One)

          Method (_STA, 0, NotSerialized)
          {
            // check for device enabled in BIOS setup, 1 = enabled
            If(LNotEqual(And(SDS0,0x01), 0x01)) {
              Return (0x00)  // device is disabled in BIOS setup
            }

            // check for device powered on, 1 = on.
            If (LNotEqual(\_SB.RDGP(46), 0x01)) {
              Return (0x00)  // device is powered off
            }

            // check for device powered on, 1 = on.
            If (LNotEqual(\_SB.RDGP(46), 0x01)) {
              Return (0x00)  // device is powered off
            }

            // check the GPIO mode select, 1 = Sensor Hub
            // verify the Sensor Hub _HID
            If(LAnd(LEqual(\_SB.RDGP(44), 0x01),LEqual(_HID, "INT33D1"))) {
              Return(0xF)  // device is Sensor Hub and Sensor Hub mode is selected
            }

            // check the GPIO mode select, 0 = DFU
            // verify the DFU _HID
            If(LAnd(LEqual(\_SB.RDGP(44), 0x00),LEqual(_HID, "INT33D7"))) {
              Return(0xF)  // device is DFU and DFU mode is selected
            }

            Return(0x00)
          }

          Method(_DSM, 0x4, NotSerialized)
          {
            // DSM UUID for HIDI2C. Do Not change.
            If(LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE")))
            {
              // Function 0 : Query Function
              If(LEqual(Arg2, Zero))
              {
                // Revision 1
                If(LEqual(Arg1, One))
                {
                  Return(Buffer(One) { 0x03 })
                }
              }
              // Function 1 : HID Function
              If(LEqual(Arg2, One))
              {
                // HID Descriptor Address (IHV Specific)
                Return(0x0001)
              }
            }
            Else
            {
              Return(Buffer(One) { 0x00 })
            }
           }

           Method (_CRS, 0, NotSerialized)
           {
              Name (SBFI, ResourceTemplate ()
              {
                I2cSerialBus (
                0x40,                  //SlaveAddress: bus address
                ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
                400000,                //ConnectionSpeed: in Hz
                AddressingMode7Bit,    //Addressing Mode: default to 7 bit
                "\\_SB.PCI0.I2C0",     //ResourceSource: I2C bus controller name
                ,                      //ResourceSourceIndex: defaults to 0
                ,                      //ResourceUsage: Defaults to ResourceConsumer
                ,                      //Descriptor Name: creates name for offset of resource descriptor
                )  //VendorData

                Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,) {28} //Route to PIRQM
              })

                Return (SBFI)
           }

