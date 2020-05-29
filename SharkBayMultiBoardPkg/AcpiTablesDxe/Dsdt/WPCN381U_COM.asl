/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved   *;
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


Device(UAR5)  // Serial Port UART 5
{
  Name(_HID, EISAID("PNP0501"))

  Name(_UID,3)

  // Status Method for UART 5.

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // not set to Disabled in BIOS Setup.

    If(W381)
    {
      // Set the SIO to UART 5.

      Store(3,WR07)

      // Display UART 5 and return status.

      If(WR30) 
      {
        Return(0x000F)
      }

      Return(0x000D)
    }

    Return(0x0000)
  }

  // Disable Method for UART 5.

  Method(_DIS,0,Serialized)
  {
    Store(3,WR07)
    Store(0,WR30)
  }

  // Current Resource Setting Method for UART 5.

  Method(_CRS,0,Serialized)
  {
    // Create the Buffer that stores the Resources to
    // be returned.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x03F8,0x03F8,0x01,0x08)
      IRQNoFlags(){4}
    })

    // Set SIO to UART 5.

    Store(3,WR07)

    // Skip this sequence if the UART 5 Port is Disabled
    // in BIOS Setup.

    If(W381)
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)

      // Write IO and Length values into the Buffer.
    
      Store(WR60,IOH0)
      Store(WR61,IOL0)
      Store(WR60,IOH1)
      Store(WR61,IOL1)
      Store(8,LEN0)

      // Write the IRQ value into the Buffer.

      And(WR70,0x0F,Local0)
      If(Local0)
      {
        ShiftLeft(One,Local0,IRQW)
      }
      Else
      {
        Store(Zero,IRQW)
      }
    }

    Return(BUF0)
  }

  // Possible Resource Setting Method for UART 5.

    // Build a Buffer with all valid COM Port Resources.

    Name(_PRS,ResourceTemplate()
    {
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3F8,0x3F8,0x01,0x08) 
        IRQNoFlags(){4}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3F8,0x3F8,0x01,0x08) 
        IRQNoFlags(){4,5,6,7,10,11,12} 
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2F8,0x2F8,0x01,0x08) 
        IRQNoFlags(){4,5,6,7,10,11,12} 
      }
    
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3E8,0x3E8,0x01,0x08) 
        IRQNoFlags(){4,5,6,7,10,11,12} 
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2E8,0x2E8,0x01,0x08) 
        IRQNoFlags(){4,5,6,7,10,11,12} 
      }
  
      EndDependentFn()
    })


  // Set Resource Setting Method for UART 5.

  Method(_SRS,1,Serialized)
  {
    // Point to the specific information in the passed
    // in Buffer.

    CreateByteField(Arg0,0x02,IOLO)
    CreateByteField(Arg0,0x03,IOHI)
    CreateWordField(Arg0,0x09,IRQW)

    // Set the SIO to UART 5.

    Store(3,WR07)

    // Disable the device.

    Store(0,WR30)

    // Set the Base IO Address.

            Store(IOLO,WR61)
            Store(IOHI,WR60)

    // Set the IRQ.

    FindSetRightBit(IRQW,Local0)
    If(LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,WR70)

    // Set the Decode Range so UART 5 works on ICH7 and
    // future platforms.

    AND(IOD0,0xF8,IOD0)     // Clear all bits.

    If(LEqual(IOHI,0x03))     // Address = 0x3xx
    {
      If(LEqual(IOLO,0xF8))   // Address = 0x3F8
      {
        Or(IOD0,0x00,IOD0)
      }
      Else        // Address = 0x3E8
      {
        Or(IOD0,0x07,IOD0)
      }
    }
    Else          // Address = 0x2xx
    {
      If(LEqual(IOLO,0xF8))   // Address = 0x2F8
      {
        Or(IOD0,0x01,IOD0)  
      }
      Else        // Address = 0x2E8
      {
        Or(IOD0,0x05,IOD0)
      }
    }

    // Enable the device.

    Store(1,WR30)
  }

  // D0 Method for COM Port.

  Method(_PS0,0,Serialized)
  {
    Store(3,WR07)
    Store(1,WR30)
  }

  // D3 Method for COM Port.

  Method(_PS3,0,Serialized)
  {
    Store(3,WR07)
    Store(0,WR30)
      }
}

Device(UAR8)  // Serial Port UART 8
{
  Name(_HID, EISAID("PNP0501"))

  Name(_UID,4)

  // Status Method for UART 8.

    Method(_STA,0,Serialized)
    {
        // Only report resources to the OS if the SIO Device is
        // not set to Disabled in BIOS Setup.

        If(W381)
        {
            And(WR27,0x0E0,Local0)
            If (LEqual (Local0, 0x00)) //SIORevID == 000x xxxxb => 374L(LDC2 card)
            {
                Store(0x2,WR07)
                // Check if Serial port 2 is enabled
                If(WR30) 
                {
                    Return(0x000F)
                }
                Return(0x000D)
            }
        }
        Return(Zero)
    }

  // Disable Method for UART 8.

  Method(_DIS,0,Serialized)
  {
    Store(2,WR07)
    Store(0,WR30)
  }

  // Current Resource Setting Method for UART 8.

  Method(_CRS,0,Serialized)
  {
    // Create the Buffer that stores the Resources to
    // be returned.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x02F8,0x02F8,0x01,0x08)
      IRQNoFlags(){3}
    })

    // Set SIO to UART 8.

    Store(2,WR07)

    // Skip this sequence if the UART 8 Port is Disabled
    // in BIOS Setup.

    If(W381)
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)

      // Write IO and Length values into the Buffer.
    
      Store(WR60,IOH0)
      Store(WR61,IOL0)
      Store(WR60,IOH1)
      Store(WR61,IOL1)
      Store(8,LEN0)

      // Write the IRQ value into the Buffer.

      And(WR70,0x0F,Local0)
      If(Local0)
      {
        ShiftLeft(One,Local0,IRQW)
      }
      Else
      {
        Store(Zero,IRQW)
      }
    }

    Return(BUF0)
  }

  // Possible Resource Setting Method for UART 8.

    // Build a Buffer with all valid UART 8 Port Resources.

    Name(_PRS,ResourceTemplate()
    {
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2F8,0x2F8,0x01,0x08) 
        IRQNoFlags(){3}
      }
    
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3F8,0x3F8,0x01,0x08) 
        IRQNoFlags(){4,5,6,7,10,11,12} 
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2F8,0x2F8,0x01,0x08) 
        IRQNoFlags(){4,5,6,7,10,11,12} 
      }
    
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3E8,0x3E8,0x01,0x08) 
        IRQNoFlags(){4,5,6,7,10,11,12} 
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2E8,0x2E8,0x01,0x08) 
        IRQNoFlags(){4,5,6,7,10,11,12} 
      }
  
      EndDependentFn()
    })


  // Set Resource Setting Method for UART 8.

  Method(_SRS,1,Serialized)
  {
    // Point to the specific information in the passed
    // in Buffer.

    CreateByteField(Arg0,0x02,IOLO)
    CreateByteField(Arg0,0x03,IOHI)
    CreateWordField(Arg0,0x09,IRQW)

    // Set the SIO to UART 8.

    Store(2,WR07)

    // Disable the device.

    Store(0,WR30)

    // Set the Base IO Address.

            Store(IOLO,WR61)
            Store(IOHI,WR60)

    // Set the IRQ.

    FindSetRightBit(IRQW,Local0)
    If(LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,WR70)

    // Set the Decode Range so UART 8 works on PCH and
    // future platforms.

    AND(IOD0,0x8F,IOD0)     // Clear all bits.

    If(LEqual(IOHI,0x03))     // Address = 0x3xx
    {
      If(LEqual(IOLO,0xF8))   // Address = 0x3F8
      {
        Or(IOD0,0x00,IOD0)
      }
      Else        // Address = 0x3E8
      {
        Or(IOD0,0x70,IOD0)
      }
    }
    Else          // Address = 0x2xx
    {
      If(LEqual(IOLO,0xF8))   // Address = 0x2F8
      {
        Or(IOD0,0x10,IOD0)  
      }
      Else        // Address = 0x2E8
      {
        Or(IOD0,0x50,IOD0)
      }
    }

    // Enable the device.

    Store(1,WR30)
  }

  // D0 Method for UART 8 Port.

  Method(_PS0,0,Serialized)
  {
    Store(2,WR07)
    Store(1,WR30)
  }

  // D3 Method for UART 8 Port.

  Method(_PS3,0,Serialized)
  {
    Store(2,WR07)
    Store(0,WR30)
      }
}
//Resource for WPCD376I SP3 Device(LDN 0x16)
Device(LD16)    // LDN 0x16
{
    Name(_HID, EISAID("PNP0501"))
    Name(_UID,5)

    // Status Method for LDN 0x16. Keep it Disabled as this LDN is part of Consumer IR device
    Method(_STA,0,Serialized)
    {
        Return(0x0000)
    }

    // Possible Resource Setting Method for LDN 0x16.
    Name(_PRS,ResourceTemplate()
    {
            StartDependentFn(0,2)
            {
                IO(Decode16,0x2F8,0x2F8,0x01,0x08) 
                IRQNoFlags(){3}
            }
            StartDependentFn(0,2)
            {
                IO(Decode16,0x3F8,0x3F8,0x01,0x08) 
                IRQNoFlags(){4,5,6,7,10,11,12} 
            }
            StartDependentFn(0,2)
            {
                IO(Decode16,0x2F8,0x2F8,0x01,0x08) 
                IRQNoFlags(){4,5,6,7,10,11,12} 
            }
            StartDependentFn(0,2)
            {
                IO(Decode16,0x3E8,0x3E8,0x01,0x08) 
                IRQNoFlags(){4,5,6,7,10,11,12} 
            }
            StartDependentFn(0,2)
            {
                IO(Decode16,0x2E8,0x2E8,0x01,0x08) 
                IRQNoFlags(){4,5,6,7,10,11,12} 
            }
            EndDependentFn()
    })
}
//Resource for WPCD376I CIR Device(LDN 0x15)
Device(LD15)    //  LDN 0x15
{
    Name(_HID, EISAID("WEC0001"))
    Name(_UID,6)

    // Status Method for LDN 0x15. Keep it Disabled as this LDN is part of Consumer IR device
    Method(_STA,0,Serialized)
    {
        Return(0x0000)
    }

    // Possible Resource Setting Method for CIR.
    Name(_PRS,ResourceTemplate()
    {
            StartDependentFn(0,2)
            {
                IO (Decode16, 0x07D0, 0x07D0, 0x01, 0x10)
                IO (Decode16, 0x07C0, 0x07C0, 0x01, 0x10)
            }
            EndDependentFn()
    })
}

//WPCD376I Consumer IR device 
Device (ECIR)
{
    Name (_HID, EISAID("WEC1022"))
    Name (_UID, One)

    OperationRegion(CIRR,SystemIO,0x7C0,0x20) 
    Field(CIRR,ByteAcc,NoLock,Preserve)
    {
        Offset(0x15), //CIR wake up base address offset 5 [0x7D0 + 5]
        RXEE, 3,
        , 5,
    }

    Method (_STA, 0, NotSerialized)
    {
        If(W381)
        {
            And(WR27,0x0E0,Local0)
            Store(0x15,WR07)
            If (LAnd(LEqual (Local0, 0x80), LOr(WR60,WR61))) //SIORevID == 100x xxxxb => 376I  and Resource allocated
            {
                If (WR30) //Check if CIR device is enabled.
                {
                    Return(0x000F)
                }
                Return(0x000D)
            }
        }
        Return(Zero)
    }

    Method (_DIS, 0, NotSerialized)
    {
        Store (0x15, WR07)
        Store (Zero, WR30)
        Store (0x16, WR07)
        Store (Zero, WR30)
     }

    Method(_CRS,0,Serialized)
    {
        // Create the Buffer that stores the Resources to
        // be returned.

        Name(BUF0,ResourceTemplate()
        {
            IO (Decode16, 0x0, 0x0, 0x01, 0x10)
            IO (Decode16, 0x0, 0x0, 0x01, 0x10)
            IO (Decode16, 0x0, 0x0, 0x08, 0x08)
            IRQNoFlags (){0}
        })

        CreateByteField(BUF0,0x02,IO00)
        CreateByteField(BUF0,0x03,IO01)
        CreateByteField(BUF0,0x04,IO10)
        CreateByteField(BUF0,0x05,IO11)
        CreateByteField(BUF0,0x0a,IO20)
        CreateByteField(BUF0,0x0b,IO21)
        CreateByteField(BUF0,0x0c,IO30)
        CreateByteField(BUF0,0x0d,IO31)
        CreateByteField(BUF0,0x12,IO40)
        CreateByteField(BUF0,0x13,IO41)
        CreateByteField(BUF0,0x14,IO50)
        CreateByteField(BUF0,0x15,IO51)
        CreateWordField(BUF0,0x19,IRQW)

           // Set the SIO to SP3.
           Store(0x16,WR07)
           // Set the Base IO Address.
           Store(WR61,IO40)
           Store(WR60,IO41)
           Store(WR61,IO50)
           Store(WR60,IO51)
           // Write the IRQ value into the Buffer.
           And(WR70,0x0F,Local0)
           If(Local0)
           {
               ShiftLeft(One,Local0,IRQW)
           }
           Else
           {
               Store(Zero,IRQW)
           }
    
           //set resource for CIr device
           // Set the SIO to SP3.
           Store(0x15,WR07)
           // Set the Base IO Address.
           Store(WR61,IO00)
           Store(WR60,IO01)
           Store(WR61,IO10)
           Store(WR60,IO11)

           Store(WR63,IO20)
           Store(WR62,IO21)
           Store(WR63,IO30)
           Store(WR62,IO31)

           Return(BUF0)
    }


    Name(_PRS,ResourceTemplate()
    {
        StartDependentFn(0,2)
        {
            IO (Decode16, 0x07D0, 0x07D0, 0x01, 0x10)
            IO (Decode16, 0x07C0, 0x07C0, 0x01, 0x10)
            IO (Decode16, 0x02F8, 0x02F8, 0x08, 0x08) 
            IRQNoFlags(){3}
        }
        StartDependentFn(0,2)
        {
            IO (Decode16, 0x07D0, 0x07D0, 0x01, 0x10)
            IO (Decode16, 0x07C0, 0x07C0, 0x01, 0x10)
            IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08) 
            IRQNoFlags(){4,5,6,7,10,11,12} 
        }
        StartDependentFn(0,2)
        {
            IO (Decode16, 0x07D0, 0x07D0, 0x01, 0x10)
            IO (Decode16, 0x07C0, 0x07C0, 0x01, 0x10)
            IO (Decode16, 0x02F8, 0x02F8, 0x08, 0x08) 
            IRQNoFlags(){4,5,6,7,10,11,12} 
        }
        StartDependentFn(0,2)
        {
            IO (Decode16, 0x07D0, 0x07D0, 0x01, 0x10)
            IO (Decode16, 0x07C0, 0x07C0, 0x01, 0x10)
            IO (Decode16, 0x03E8, 0x03E8, 0x08, 0x08) 
            IRQNoFlags(){4,5,6,7,10,11,12} 
        }
        StartDependentFn(0,2)
        {
            IO (Decode16, 0x07D0, 0x07D0, 0x01, 0x10)
            IO (Decode16, 0x07C0, 0x07C0, 0x01, 0x10)
            IO (Decode16, 0x02E8, 0x02E8, 0x08, 0x08) 
            IRQNoFlags(){4,5,6,7,10,11,12} 
        }
        EndDependentFn ()
    })

    Method (_SRS, 1, NotSerialized)
    {
        CreateByteField(Arg0,0x02,IO00)
        CreateByteField(Arg0,0x03,IO01)
        CreateByteField(Arg0,0x0a,IO20)
        CreateByteField(Arg0,0x0b,IO21)
        CreateByteField(Arg0,0x12,IO40)
        CreateByteField(Arg0,0x13,IO41)
        CreateWordField(Arg0,0x19,IRQW)

        // Set the SIO to SP3.
        Store(0x16,WR07)
        // Set the Base IO Address.
        Store(IO40, WR61)
        Store(IO41, WR60)
        // Set the IRQ.
        FindSetRightBit(IRQW,Local0)
        If(LNotEqual(IRQW,Zero))
        {
            Decrement(Local0)
        }
        Store(Local0,WR70)
    
        AND(IOD0,0x8F,IOD0)            // Clear all bits.
        If(LEqual(IO41,0x03))            // Address = 0x3xx
        {
            If(LEqual(IO40,0xF8))        // Address = 0x3F8
            {
                Or(IOD0,0x00,IOD0)
            }
            Else                // Address = 0x3E8
            {
                Or(IOD0,0x70,IOD0)
            }
        }
        Else                    // Address = 0x2xx
        {
            If(LEqual(IO40,0xF8))        // Address = 0x2F8
            {
                Or(IOD0,0x10,IOD0)    
            }
                Else                // Address = 0x2E8
            {
                Or(IOD0,0x50,IOD0)
            }
        }

        //set resource for CIr device
        // Set the SIO to SP3.
        Store(0x15,WR07)
        // Set the Base IO Address.
        Store(IO00, WR61)
        Store(IO01, WR60)
        Store(IO20, WR63)
        Store(IO21, WR62)
        Store (0x15, WR07)
        Store (One, WR30)
        Store (0x16, WR07)
        Store (0x82, WRF0)
        Store (One, WR30)
    }

    Method (_S3D, 0, NotSerialized)
    {
        Return (0x02)
    }

    Method (_S4D, 0, NotSerialized)
    {
        Return (0x02)
    }

    Method (_S3W, 0, NotSerialized)
    {
        Return (0x02)
    }

    Method (_S4W, 0, NotSerialized)
    {
        Return (0x02)
    }

    Name (CPS4, Package (0x02)
    {
        0x1D, 
        0x04
    })
    Method (_PRW, 0, NotSerialized)
    {
        Return (CPS4)
    }

    Method (_PSW, 1, NotSerialized)
    {
        If (LEqual (Arg0, Zero))
        {
            Store (Zero, RXEE)
        }
        If (LEqual (Arg0, One))
        {
            Store (0x07, RXEE)
        }
    }
}
