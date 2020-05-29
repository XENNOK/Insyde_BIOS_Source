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

Scope (\_SB.PCI0)
{
      External(\_SB.PCI0.SDHC.WI01.PS0X, MethodObj)
      External(\_SB.PCI0.SDHC.WI01.PS3X, MethodObj)
      External(\_SB.PCI0.I2C1.TPD7.PS0X, MethodObj)
      External(\_SB.PCI0.I2C1.TPD8.PS0X, MethodObj)


} // end scope \_SB.PCI0

Scope(\_SB.PCI0.I2C0)
{

        //--------------------
        //  Audio Codec device
        //  Realtek
        //--------------------
        Device (ACD0)
        {
          // I2C/I2S Audio Codec (Realtek as default)
          Name (_ADR, 0x1C)
          Name (_HID, "INT33CA")
          Name (_CID, "INT33CA")
          Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec - INT33CA" )
          Name (_UID, 1)

          // Parameter values for Realtek codec
          Name (MCLK, Zero)
          Name (SCLK, 0x9)
          Name (SSPM, Zero)

          Name (RBUF, ResourceTemplate ()
          {
              I2CSerialBus(
              0x1C,                      //SlaveAddress: bus address
              ,                          //SlaveMode: default to ControllerInitiated
              400000,                    //ConnectionSpeed: in Hz
              ,                          //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C0",         //ResourceSource: I2C bus controller name
              ,                          //Descriptor Name: creates name for offset of resource descriptor
              )                          //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, ExclusiveAndWake, , ,) {37} //Route to PIRQx - jack detection
          })

          Name(EOD, 1)
          Method (_CRS, 0x0, NotSerialized)
          {
            Return (RBUF)
          }

          Method (_SRS, 0x1, Serialized)
          {
            Store (1, EOD)
          }

          Method (_STA, 0x0, NotSerialized)
          {
            If (LOr(LNotEqual(CODS, Zero), LNotEqual(ADSD, Zero)))
            {
              Return(0x0)  // Codec Selection != Realtek or ADSP disabled - Not present
            }
            If (AND(EOD, 0x1, EOD))
            {
              Return(0xf)  // Enabled 1111
            }
            Else
            {
              Return(0xd)  // Disabled 1101
            }
          }

          Method (_DIS, 0x0, NotSerialized)
          {
            Store(0, EOD)
          }
        }   //  Device (ACD0)

        //--------------------
        //  Audio Codec device
        //  Cirrus Logic
        //--------------------
        Device (ACD1)
        {
          Name (_ADR, 0x4A)
          Name (_HID, "INT33C9")
          Name (_CID, "INT33C9")
          Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec - INT33C9" )
          Name (_UID, 1)

          // Parameter values for Cirrus codec
          Name (MCLK, 0x6)
          Name (SCLK, Zero)
          Name (SSPM, One)

          Name (RBUF, ResourceTemplate ()
          {
            I2CSerialBus(
            0x4A,                      //SlaveAddress: bus address
            ,                          //SlaveMode: default to ControllerInitiated
            400000,                    //ConnectionSpeed: in Hz
            ,                          //Addressing Mode: default to 7 bit
            "\\_SB.PCI0.I2C0",         //ResourceSource: I2C bus controller name
            ,                          //Descriptor Name: creates name for offset of resource descriptor
            )                          //VendorData
          })

          Name(EOD, 1)
          Method (_CRS, 0x0, NotSerialized)
          {
            Store (1, EOD)
            Return (RBUF)
          }

          Method (_STA, 0x0, NotSerialized)
          {
            If (LOr(LNotEqual(CODS, One), LNotEqual(ADSD, Zero)))
            {
              Return(0x0)  // Codec Selection != Cirrus or ADSP disabled - Not present
            }
            If (AND(EOD, 0x1, EOD))
            {
              Return(0xf)  // Enabled 1111
            }
            Else
            {
              Return(0xd)  // Disabled 1101
            }
          }

          Method (_DIS, 0x0, NotSerialized)
          {
            Store(0, EOD)
          }
        }   //  Device (ACD1)

        //--------------------
        //  Audio Codec device
        //  IDT
        //--------------------
        Device (ACD2)
        {
          Name (_ADR, 0x69)
          Name (_HID, "INT33CB")
          Name (_CID, "INT33CB")
          Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec - INT33CB" )
          Name (_UID, 1)

          // Parameter values for IDT codec
          Name (MCLK, 0x18)
          Name (SCLK, 0x9)
          Name (SSPM, Zero)

          Name (RBUF, ResourceTemplate ()
          {
            I2CSerialBus(
            0x69,                      //SlaveAddress: bus address
            ,                          //SlaveMode: default to ControllerInitiated
            400000,                    //ConnectionSpeed: in Hz
            ,                          //Addressing Mode: default to 7 bit
            "\\_SB.PCI0.I2C0",         //ResourceSource: I2C bus controller name
            ,                          //Descriptor Name: creates name for offset of resource descriptor
            )                          //VendorData

           GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionInputOnly, "\\_SB.PCI0.GPI0", ) {0x33} // GPIO51 for HP detection
           GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionInputOnly, "\\_SB.PCI0.GPI0", ) {0x35} // GPIO53 for Mic detection

           Interrupt(ResourceConsumer, Edge, ActiveHigh, ExclusiveAndWake, , ,) {35} // GPIO51 - HP jack detection
           Interrupt(ResourceConsumer, Edge, ActiveHigh, ExclusiveAndWake, , ,) {37} // GPIO53 - Mic jack detection
          })

          Name(EOD, 1)
          Method (_CRS, 0x0, NotSerialized)
          {
            Return (RBUF)
          }

          Method (_SRS, 0x1, Serialized)
          {
            Store (1, EOD)
          }

          Method (_STA, 0x0, NotSerialized)
          {
            If (LOr(LNotEqual(CODS, 2), LNotEqual(ADSD, Zero)))
            {
              Return(0x0)  // Codec Selection != IDT or ADSP disabled - Not present
            }
            If (AND(EOD, 0x1, EOD))
            {
              Return(0xf)  // Enabled 1111
            }
            Else
            {
              Return(0xd)  // Disabled 1101
            }
          }

          Method (_DIS, 0x0, NotSerialized)
          {
            Store(0, EOD)
          }
        }   //  Device (ACD2)

        //--------------------
        //  Intel Sensor Hub
        //--------------------
        Device (SHUB)
        {
          Name (_HID, "INT33D1")
          Name (_CID, "PNP0C50")
          Include ("Sensor.asl")
        }  // Device (SHUB)

        //--------------------
        //  Intel DFU Device
        //--------------------
        Device (DFUD)
        {
          Name (_HID, "INT33D7")
          Include ("Sensor.asl")
        } // Device DFUD

        Device (TPD4)
        {
          Name (_ADR, Zero)
          Name (_HID, "MSFT1111")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

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

           Method (_STA, 0, NotSerialized)
           {
             If(LEqual(And(SDS0,0x04), 0x04)) {
               Return (0x0F)
             } Else {
               Return (0x00)
             }
           }

           Method (_CRS, 0, NotSerialized)
           {
              Name (SBFI, ResourceTemplate ()
              {
                I2cSerialBus (
                0x60,                  //SlaveAddress: bus address
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

          Method (_PRW, 0) {
            Return(Package(){0x0E,4}) // can wakeup from S4 state
          }

          Method (_S3W, 0) {
            Return(3)
          }

          Method (_S4W, 0) {
           Return(3)
          }

          Method(_PS0,0,Serialized)
          {
            ADBG("TPD4 Ctrlr D0")
            //
            // Set GPI_INV to normal
            //
            \_SB.WTIN(14,0)

           //
           // Set GPI_OWN to GPIO
           //
            Store(1,\GO14)
          }

          Method(_PS3,0,Serialized)
          {
            ADBG("TPD4 Ctrlr D3")
            //
            // Set GPI_INV to Inverted
            //
            \_SB.WTIN(14,1)

            //
            // Set GPI_OWN to ACPI
            //
            Store(0,\GO14)
          }
        }  // Device (TPD4)


}  //  end Scope(\_SB.PCI0.I2C0)


      //----------------------------
      //  Serial IO I2C1 Controller
      //----------------------------
Scope(\_SB.PCI0.I2C1)
{


        //------------------------
        //  Atmel Touch Panel
        //------------------------
        Device (TPL0)
        {
          Name (_ADR, Zero)
          Name (_HID, "ATML1000")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 4)          // required to put the device to D3 Cold during S0 idle

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
                Else
                {
                  Return(Buffer(One) { 0x00 })
                }
              }
              // Function 1 : HID Function
              If(LEqual(Arg2, One))
              {
                // HID Descriptor Address (IHV Specific)
                Return(0x0000)
              }
            }
            Else
            {
              Return(Buffer(One) { 0x00 })
            }
          }

          Method (_STA, 0, NotSerialized)
          {
            // Note:  _STA in ATML1000 and ATML2000 must be identical.  These devices
            // communicate with the same HW.

            If(LEqual(And(SDS1,0x0001), 0x0001)) {
               Return (0x0F)
            } Else {
               Return (0x00)
             }
          }

          Method (_CRS, 0, NotSerialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x4C,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,) {34}    //Interrupt from touch screen routed to GPIO50-->PIRQS
            })

            Return (SBFI)
          }
        }  // Device (TPL0)

        //------------------------
        //  Atmel Touch Panel FW Update
        //------------------------
        Device (TPFU)
        {
          Name (_ADR, Zero)
          Name (_HID, "ATML2000")
          Name (_CID, "PNP0C02")
          Name (_UID, 10)
             
          Method (_STA, 0, NotSerialized)
          {
            // Note:  _STA in ATML1000 and ATML2000 must be identical.  These devices
            // communicate with the same HW.
            
            If(LAnd(And(SDS1,0x0001),And(APFU,0x0001))) {
               Return (0x0F)
            } Else {
               Return (0x00)
             }
          }

          Method (_CRS, 0, NotSerialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x26,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              I2cSerialBus (
              0x27,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData
           })

            Return (SBFI)
          }
        }  // Device (TPFU)

        //------------------------
        //  ELAN Touch Panel
        //------------------------
        Device (TPL1)
        {
          Name (_HID, "ELAN1001")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 4)          // required to put the device to D3 Cold during S0 idle

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
                Else
                {
                  Return(Buffer(One) { 0x00 })
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

          Method (_STA, 0, NotSerialized)
          {
            If(LEqual(And(SDS1,0x0002), 0x0002)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }

          Method (_CRS, 0, NotSerialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x10,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,) {34}    //Interrupt from touch screen routed to GPIO50-->PIRQS
            })

            Return (SBFI)
          }
        }  // Device (TPL1)


        //------------------------------
        //  NTRIG Digitizer Touch Panel
        //------------------------------
        Device (TPL2)
        {
          Name (_ADR, One)
          Name (_HID, "NTRG0001")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 4)          // required to put the device to D3 Cold during S0 idle

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
                Else
                {
                  Return(Buffer(One) { 0x00 })
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

          Method (_STA, 0, NotSerialized)
          {
            If(LEqual(And(SDS1,0x0020), 0x0020)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }

          Method (_CRS, 0, NotSerialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x07,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,) {34}    //Interrupt from touch screen routed to GPIO50-->PIRQS
            })

            Return (SBFI)

          }
        }  // Device (TPL2)

        //------------------------
        //  EETI Touch Panel
        //------------------------
        Device (TPL3)
        {
          Name (_ADR, One)
          Name (_HID, "EETI7900")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 4)          // required to put the device to D3 Cold during S0 idle

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
                Else
                {
                  Return(Buffer(One) { 0x00 })
                }
              }
              // Function 1 : HID Function
              If(LEqual(Arg2, One))
              {
                // HID Descriptor Address (IHV Specific)
                Return(0x000F)
              }
            }
            Else
            {
              Return(Buffer(One) { 0x00 })
            }
          }

          Method (_STA, 0, NotSerialized)
          {
            If(LEqual(And(SDS1,0x0040), 0x0040)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }

          Method (_CRS, 0, NotSerialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x2A,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,) {34}    //Interrupt from touch screen routed to GPIO50-->PIRQS
            })


            Return (SBFI)

          }
        }  // Device (TPL3)


        //------------------------
        //  ELAN Touch Pad
        //------------------------
        Device (TPD0)
        {
          Name (_ADR, One)
          Name (_HID, "ELAN1000")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 4)          // required to put the device to D3 Cold during S0 idle

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
                Else
                {
                  Return(Buffer(One) { 0x00 })
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

          Method (_STA, 0, NotSerialized)
          {
            If(LEqual(And(SDS1,0x0004), 0x0004)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }

          Method (_CRS, 0, Serialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x15,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,INT0) {39} //Interrupt from Touchpad routed to GPIO55 - IOxAPIC PIRQX
            })

            If(LEqual(GR13, 1)) {
              CreateByteField (SBFI, INT0._INT, VAL3) // Extended Interrupt Descriptor
              Store(27, VAL3)
            }

                Return (SBFI)


          }
        }  // Device (TPD0)

        //------------------------
        //  Synaptics touchpad
        //------------------------
        Device (TPD1)
        {
          Name (_ADR, One)
          Name (_HID, "MSFT0001")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 4)          // required to put the device to D3 Cold during S0 idle

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
                Else
                {
                  Return(Buffer(One) { 0x00 })
                }
              }
              // Function 1 : HID Function
              If(LEqual(Arg2, One))
              {
                // HID Descriptor Address (IHV Specific)
                Return(0x0020)
              }
            }
            Else
            {
              Return(Buffer(One) { 0x00 })
            }
          }

          Method (_STA, 0, NotSerialized)
          {
            If(LEqual(And(SDS1,0x0008), 0x0008)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }

          Method (_CRS, 0, Serialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x20,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,INT0) {39} //Interrupt from Touchpad routed to GPIO55 - IOxAPIC PIRQX
            })

            If(LEqual(GR13, 1)) {
              CreateByteField (SBFI, INT0._INT, VAL3) // Extended Interrupt Descriptor
              Store(27, VAL3)
            }

                Return (SBFI)
          }
        }  // Device (TPD1)

        //------------------------
        //  Alps touchpad
        //------------------------
        Device (TPD2)
        {
          Name (_ADR, One)
          Name (_HID, "ALP0001")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 4)          // required to put the device to D3 Cold during S0 idle

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
                Else
                {
                  Return(Buffer(One) { 0x00 })
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

          Method (_STA, 0, NotSerialized)
          {
            If(LEqual(And(SDS1,0x0080), 0x0080)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }

          Method (_CRS, 0, Serialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x2A,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,INT0) {39} //Interrupt from Touchpad routed to GPIO55 - IOxAPIC PIRQX
            })

            If(LEqual(GR13, 1)) {
              CreateByteField (SBFI, INT0._INT, VAL3) // Extended Interrupt Descriptor
              Store(27, VAL3)
            }

                Return (SBFI)
          }
        }  // Device (TPD2)

        //------------------------
        //  Cypress touchpad
        //------------------------
        Device (TPD3)
        {
          Name (_ADR, One)
          Name (_HID, "CYP0001")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 4)          // required to put the device to D3 Cold during S0 idle

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
                Else
                {
                  Return(Buffer(One) { 0x00 })
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

          Method (_STA, 0, NotSerialized)
          {
            If(LEqual(And(SDS1,0x0100), 0x0100)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }

          Method (_CRS, 0, Serialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x24,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,INT0) {39} //Interrupt from Touchpad routed to GPIO55 - IOxAPIC PIRQX
            })

            If(LEqual(GR13, 1)) {
              CreateByteField (SBFI, INT0._INT, VAL3) // Extended Interrupt Descriptor
              Store(27, VAL3)
            }

                Return (SBFI)
          }
        }  // Device (TPD3)

        //------------------------
        //  ELAN Precision Touch Pad
        //------------------------
        Device (TPD7)
        {
          Name (_ADR, One)
          Name (_HID, "ELAN1010")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 3)          // PTP will be in D3hot during CS, and wake capable

          Method (_S3W, 0) {
            If(LEqual(S0ID, 0)) {
              Return (3)
            } Else {
              Return (0)
            }
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
                Else
                {
                  Return(Buffer(One) { 0x00 })
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

	  // Get the right SDS value
          Method (_STA, 0, NotSerialized)
          {
            If(LEqual(And(SDS1,0x0800), 0x0800)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }

          Method (_CRS, 0, Serialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x15,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData

              Interrupt(ResourceConsumer, Level, ActiveLow, ExclusiveAndWake, , ,INT0) {39} //Interrupt from Touchpad routed to GPIO55 - IOxAPIC PIRQX
            })

            If(LEqual(GR13, 1)) {
              CreateByteField (SBFI, INT0._INT, VAL3) // Extended Interrupt Descriptor
              Store(27, VAL3)

              If(LEqual(S0ID, 0)) {
                CreateByteField (SBFI, INT0._SHR, VAL4) // Change to exclusive
                And(VAL4, 0xE7, VAL4) //Exclusive
              }
            }

                Return (SBFI)


          }

          Method (_PRW, 0) {
            If(LAnd(LEqual(S0ID, 0),LEqual(GR13, 1))) {
              Return(Package(){0x0D, 3}) // can wakeup from S4 state
            }
            Return(Package(){Zero, Zero})
          }

          Method(_PS0,0,Serialized)
          {
            ADBG("TPD7 Ctrlr D0")
            If(LAnd(LEqual(S0ID, 0),LEqual(GR13, 1))) {
              \_SB.WTIN(13, 0) // Set GPI_INV to normal
              Store(1, \GO13) // Set GPI_OWN to GPIO
            }

            If(CondRefOf(\_SB.PCI0.I2C1.TPD7.PS0X))
            {
              \_SB.PCI0.I2C1.TPD7.PS0X()
            }
          }

          Method(_PS3,0,Serialized)
          {
            ADBG("TPD7 Ctrlr D3")
            If(LAnd(LEqual(S0ID, 0),LEqual(GR13, 1))) {
              \_SB.WTIN(13, 1) // Set GPI_INV to Inverted
              Store(0, \GO13) // Set GPI_OWN to ACPI
            }
          }
        }  // Device (TPD7)

        //------------------------
        //  Synaptics Precision touchpad
        //------------------------
        Device (TPD8)
        {
          Name (_ADR, One)
          Name (_HID, "SYNA2393")
          Name (_CID, "PNP0C50")
          Name (_UID, One)

          Name (_S0W, 3)          // PTP will be in D3hot during CS, and wake capable

          Method (_S3W, 0) {
            If(LEqual(S0ID, 0)) {
              Return (3)
            } Else {
              Return (0)
            }
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
                Else
                {
                  Return(Buffer(One) { 0x00 })
                }
              }
              // Function 1 : HID Function
              If(LEqual(Arg2, One))
              {
                // HID Descriptor Address (IHV Specific)
                Return(0x0020)
              }
            }
            Else
            {
              Return(Buffer(One) { 0x00 })
            }
          }

          // Get the right SDS value
          Method (_STA, 0, NotSerialized)
          {
            If(LEqual(And(SDS1,0x1000), 0x1000)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }

          Method (_CRS, 0, Serialized)
          {
            Name (SBFI, ResourceTemplate ()
            {
              I2cSerialBus (
              0x20,                  //SlaveAddress: bus address
              ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
              400000,                //ConnectionSpeed: in Hz
              AddressingMode7Bit,    //Addressing Mode: default to 7 bit
              "\\_SB.PCI0.I2C1",     //ResourceSource: I2C bus controller name
              ,                      //ResourceSourceIndex: defaults to 0
              ,                      //ResourceUsage: Defaults to ResourceConsumer
              ,                      //Descriptor Name: creates name for offset of resource descriptor
              )  //VendorData
              
              Interrupt(ResourceConsumer, Level, ActiveLow, ExclusiveAndWake, , ,INT0) {39} //Interrupt from Touchpad routed to GPIO55 - IOxAPIC PIRQX
            })

            If(LEqual(GR13, 1)) {
              CreateByteField (SBFI, INT0._INT, VAL3) // Extended Interrupt Descriptor
              Store(27, VAL3) //PIRQ

              If(LEqual(S0ID, 0)) {
                CreateByteField (SBFI, INT0._SHR, VAL4) // Change to exclusive
                And(VAL4, 0xE7, VAL4) //Exclusive
              }
            }

                Return (SBFI)
          }

          Method (_PRW, 0) {
            If(LAnd(LEqual(S0ID, 0),LEqual(GR13, 1))) {
              Return(Package(){0x0D,3}) // can wakeup from S4 state
            }
            Return(Package(){Zero, Zero})
          }

          Method(_PS0,0,Serialized)
          {
            ADBG("TPD8 Ctrlr D0")
            If(LAnd(LEqual(S0ID, 0),LEqual(GR13, 1))) {
              \_SB.WTIN(13, 0) // Set GPI_INV to normal
              Store(1, \GO13) // Set GPI_OWN to GPIO
            }

            If(CondRefOf(\_SB.PCI0.I2C1.TPD8.PS0X))
            {
              \_SB.PCI0.I2C1.TPD8.PS0X()
            }
          }

          Method(_PS3,0,Serialized)
          {
            ADBG("TPD8 Ctrlr D3")
            If(LAnd(LEqual(S0ID, 0),LEqual(GR13, 1))) {
              \_SB.WTIN(13, 1) // Set GPI_INV to Inverted
              Store(0, \GO13) // Set GPI_OWN to ACPI
            }
          }
        }  // Device (TPD8)
      }  //  Device (I2C1)

      //----------------------------
      //  Serial IO SPI0 Controller
      //----------------------------
Scope(\_SB.PCI0.SPI0)
{


}   //  end Scope(\_SB.PCI0.SPI0)

      //----------------------------
      //  Serial IO SPI1 Controller
      //----------------------------
Scope(\_SB.PCI0.SPI1)
{

}   //  end Scope(\_SB.PCI0.SPI1)

      //-----------------------------
      //  Serial IO UART0 Controller
      //-----------------------------
Scope(\_SB.PCI0.UA00)
{

        //
        // Bluetooth controller using serial interface
        //
        Device(BTH0)
        {
          Name(_HID, "INT33E0")

          Method(_CRS, 0x0, NotSerialized)
          {
            Name(UBUF, ResourceTemplate ()
            {
              UARTSerialBus(
              115200,              //InitialBaudRate: in bits per second
              DataBitsEight,       //BitsPerByte: default to DataBitsEight (optional)
              StopBitsOne,         //StopBits: defaults to StopBitsOne (optional)
              0xc0,                //LinesInUse: 8 1-bit flags to declare line enabled
              ,                    //IsBigEndian: default to LittleEndian (optional)
              ParityTypeNone,      //Parity: defaults to ParityTypeNone (optional)
              FlowControlHardware, //FlowControl: defaults to FlowControlNone (optional)
              32,                  //ReceiveBufferSize
              32,                  //TransmitBufferSize
              "\\_SB.PCI0.UA00",   //ResourceSource: UART bus controller name
              ,                    //ResourceSourceIndex: defaults to 0 (optional)
              ,                    //ResourceUsage: defaults to ResourceConsumer (optional)
              ,                    //DescriptorName: creates name for offset of resource descriptor
              )                    //VendorData
            })

            Return (UBUF)
          }

          Method (_STA, 0x0, NotSerialized)
          {
            If(LEqual(And(SDS4,0x01), 0x01)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }
        } // Device BTH0

      }  //  end Scope(\_SB.PCI0.UART0)

      //-----------------------------
      //  Serial IO UART1 Controller
      //-----------------------------
Scope(\_SB.PCI0.UA01)
      {

        //
        // Bluetooth controller using serial interface
        //
        Device(BTH1)
        {
          Name(_HID, "INT33E0")

          Method(_CRS, 0x0, NotSerialized)
          {
            Name(UBUF, ResourceTemplate ()
            {
              UARTSerialBus(
              115200,              //InitialBaudRate: in bits per second
              DataBitsEight,       //BitsPerByte: default to DataBitsEight (optional)
              StopBitsOne,         //StopBits: defaults to StopBitsOne (optional)
              0xc0,                //LinesInUse: 8 1-bit flags to declare line enabled
              ,                    //IsBigEndian: default to LittleEndian (optional)
              ParityTypeNone,      //Parity: defaults to ParityTypeNone (optional)
              FlowControlHardware, //FlowControl: defaults to FlowControlNone (optional)
              32,                  //ReceiveBufferSize
              32,                  //TransmitBufferSize
              "\\_SB.PCI0.UA01",   //ResourceSource: UART bus controller name
              ,                    //ResourceSourceIndex: defaults to 0 (optional)
              ,                    //ResourceUsage: defaults to ResourceConsumer (optional)
              ,                    //DescriptorName: creates name for offset of resource descriptor
              )                    //VendorData
              Interrupt(ResourceConsumer, Level, ActiveLow, SharedAndWake, , , ) {25} // GPIO9 - PIRQ J(APIC pin 25) for BT Wake
              GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PCI0.GPI0", ) {87} // for RF Kill
            })

            Return (UBUF)
          }

          Method (_STA, 0x0, NotSerialized)
          {
            If(LEqual(And(SDS5,0x01), 0x01)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }
        } // Device BTH1

        //
        // BroadCom Bluetooth controller using serial interface
        //
        Device(BTH2)
        {
          Method(_HID, 0, NotSerialized) {
            if (LEqual(BCV4, 0)) {
              Return("BCM2E20")
            } else {
              Return("BCM2E40")
            }
          }

          Method(_CRS, 0x0, NotSerialized)
          {
            Name(UBUF, ResourceTemplate ()
            {
              UARTSerialBus(
              115200,              //InitialBaudRate: in bits per second
              ,                    //BitsPerByte: default to DataBitsEight (optional)
              ,                    //StopBits: defaults to StopBitsOne (optional)
              0xc0,                //LinesInUse: 8 1-bit flags to declare line enabled
              ,                    //IsBigEndian: default to LittleEndian (optional)
              ,                    //Parity: defaults to ParityTypeNone (optional)
              FlowControlHardware, //FlowControl: defaults to FlowControlNone (optional)
              32,                  //ReceiveBufferSize
              32,                  //TransmitBufferSize
              "\\_SB.PCI0.UA01",   //ResourceSource: UART bus controller name
              ,                    //ResourceSourceIndex: defaults to 0 (optional)
              ,                    //ResourceUsage: defaults to ResourceConsumer (optional)
              ,                    //DescriptorName: creates name for offset of resource descriptor
              )                    //VendorData
              Interrupt(ResourceConsumer, Edge, ActiveLow, Exclusive, , , ) {25} // GPIO9 - PIRQ J(APIC pin 25) for BT Wake
              GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PCI0.GPI0", ) {57} // for BT_DEV_WAKE
              GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PCI0.GPI0", ) {87} // for RF Kill
            })
            Return (UBUF)
          }

          Method (_STA, 0x0, NotSerialized)
          {
            If(LEqual(And(SDS5,0x02), 0x02)) {
               Return (0x0F)
            } Else {
               Return (0x00)
            }
          }
          Name (_S0W, 2)                            // required to put the device to D2 during S0 idle
        } // Device BTH2


      }  // end Scope(\_SB.PCI0.UART1)

      //--------------------------------
      // Serial IO SDIO Host Controller
      //--------------------------------
Scope(\_SB.PCI0.SDHC)
{

        Device (WI01)
        {
          Name (_ADR, 1)
          Name (_DDN, "SDIO Wifi device Function 1" )

          Method (_STA, 0x0, NotSerialized)
          {
            Return (0x0F)
          }

          Method (_RMV, 0, NotSerialized)
          {
            Return (0x0)
          }

          Name (_S4W, 2)                            // required per guidance from MS (SDIO Wifi device power states doc)
          Name (_S0W, 2)                            // required to put the Wifi device to D2 during S0 idle
          // D0 Method for WiFi
          Method(_PS0,0,Serialized)
          {
            ADBG("WiFi1 Enter D0")
            If(CondRefOf(\_SB.PCI0.SDHC.WI01.PS0X))
            {
              \_SB.PCI0.SDHC.WI01.PS0X()
            }
          }
          // D2 Method for WiFi
          Method(_PS2,0,Serialized)
          {
            ADBG("WiFi1 Enter D2")
          }
          // D3 Method for WiFi
          Method(_PS3,0,Serialized)
          {
            ADBG("WiFi1 Enter D3")
            If(CondRefOf(\_SB.PCI0.SDHC.WI01.PS3X))
            {
              \_SB.PCI0.SDHC.WI01.PS3X()
            }
          }
          //Method(_DSW, 3){
            //ADBG("Wifi1 _DSW")
          //}
          //
          Name (RBUF, ResourceTemplate ()
          {
            Memory32Fixed (ReadWrite, 0x00000000, 0x0000000, BARC)  // SW LTR Registers
            Interrupt(ResourceConsumer, Level, ActiveLow, SharedAndWake, , , ) {38}
          })

          Method (_CRS, 0, NotSerialized)
          {
            CreateDWordField(^^RBUF, ^^BARA._BAS, AVAL)
            if(LNotEqual(AVAL, 0)) {
              CreateDWordField(^RBUF, ^BARC._LEN, WLN0)
              Store(0xC, WLN0)
              CreateDWordField(^RBUF, ^BARC._BAS, WVAL)
              Add(AVAL, 0x1008, WVAL)
            }
            Return (RBUF)
          }
        }
}   //  end Scope(\_SB.PCI0.SDHC)

