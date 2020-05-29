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



// This ASL file will support the LPT in Output, BiDirectional, EPP,
// & ECP mode.  Only 1 of these devices will be shown by the OS at any
// given time, and that one will depend on the BIOS configuration.

Device(LPT1)  // Printer, Output Mode
{
  Name(_HID,EISAID("PNP0400"))
  Name(_UID,1)

  // Status Method for LPT-Output Mode.

  Method(_STA,0,Serialized) 
  {
    // Output mode = 000xxxxxb - check that no bits are set.
    Store(1,R07H)
    If(LEqual(And(RF0H,0xE0),0x00))
    {
      // LPT is in Output Mode.  Only report resources
      // to the OS if the SIO Device is not set to
      // Disabled in BIOS Setup.
      
      If(LAnd(NATP,LPTP)) 
      {
        // Set SIO to LPT.


        // Display POUT and return Status.

        If(R30H)
        {
          Return(0x000F)
        }

        Return(0x000D)
      }
    }

    Return(0x0000)
  }

  // Disable Method for LPT-Output Mode.

  Method(_DIS,0,Serialized)
  {
    // Set SIO to LPT.

    Store(1,R07H)

    If(LEqual(And(RF0H,0xE0),0x00))
    {
      Store(0,R30H)
    } 
  }
   
  // Current Resource Setting Method for LPT-Output Mode.

  Method(_CRS,0,Serialized)
  {
    // Create a buffer to return to OS.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x0378,0x0378,0x01,0x04)     
      IRQNoFlags(){7}
    })

    // Skip this sequence if the LPT Device is not set
    // to Output Mode.

    If(LEqual(And(RF0H,0xE0),0x00))
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)
  
      // Set SIO to LPT.

      Store(1,R07H)

      // Write IO and Length values into the Buffer.
    
      Store(R61H,IOL0)
      Store(R60H,IOH0)
      Store(IOL0,IOL1)
      Store(IOH0,IOH1)
      Store(4,LEN0)

      // Write the IRQ value into the Buffer.

      If(And(R70H,0x0F))
      {
        ShiftLeft(One,And(R70H,0x0F),IRQW)
      }
      Else
      {
        Store(Zero,IRQW)
      }
    }

    Return(BUF0) 
  }

  // Possible Resource Setting Method for LPT-Output Mode.

    // Build a Buffer with all valid LPT Port Resources.

    Name(_PRS,ResourceTemplate()
    {
      StartDependentFn(0,2)
      {
        IO(Decode16,0x378,0x378,0x01,0x04)
        IRQNoFlags(){7}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x278,0x278,0x01,0x04)
        IRQNoFlags(){7}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3BC,0x3BC,0x01,0x04)
        IRQNoFlags(){7}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x378,0x378,0x01,0x04)
        IRQNoFlags(){5}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x278,0x278,0x01,0x04)
        IRQNoFlags(){5}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3BC,0x3BC,0x01,0x04)
        IRQNoFlags(){5}
      }

      EndDependentFn()
    })
    

  // Set Resource Setting Method for LPT-Output Mode.

  Method(_SRS,1,Serialized) 
  {
    // Point to the specific information in the passed
    // in Buffer.

    CreateByteField(Arg0,0x02,IOL0)
    CreateByteField(Arg0,0x03,IOH0)
    CreateWordField(Arg0,0x09,IRQW)

    // Set SIO to LPT.

    Store(1,R07H)

    // Disable the LPT.

    Store(0,R30H)

    // Set to Output Mode.

    And(RF0H,0x0F,RF0H)

    // Disable the DMA channels.

            Store(4,R74H)
    
    // Set the Base IO Address.
    
            Store(IOL0,R61H)
            Store(IOH0,R60H)

    // Set the IRQ.

    FindSetRightBit(IRQW,Local0)
    If (LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,R70H)

    // Set the Decode Range so POUT works when docked.

    And(IOD1,0xFC,IOD1)     // Decode all bits.

    If(LEqual(IOH0,0x03))     // Address = 0x3xx
    {
      If(LEqual(IOL0,0x78))   // Address = 0x378
      {
        Or(IOD1,0x00,IOD1)
      }
      Else
      {
        Or(IOD1,0x02,IOD1)  // Address = 0x3BC
      }
    }
    Else          // Address = 0x278
    {
      Or(IOD1,0x01,IOD1)
    }

    // Enable the LPT.

    Store(1,R30H)
  }

  // D0 Method for LPT-Output Mode.

  Method(_PS0,0,Serialized)
  {
    Store(1,R07H)
    Store(1,R30H)
  }

  // D3 Method for LPT-Output Mode.

  Method(_PS3,0,Serialized)
  {
    Store(1,R07H)
    Store(0,R30H)
      }
}

Device(PBI1)  // Printer, BiDirectional Mode
{
  Name(_HID,EISAID("PNP0400"))
  Name(_UID,2)

  // Status Method for LPT-BiDirectional Mode.

  Method(_STA,0,Serialized) 
  {
    // BiDirectional mode = 001xxxxxb, so check that only 
    // bit 5 is set.

    Store(1,R07H)
    If(LEqual(And(RF0H,0xE0),0x20))
    {
      // LPT is in BiDirectional Mode.  Only report
      // resources to the OS if the SIO Device is not
      // set to Disabled in BIOS Setup.

      If(LAnd(NATP,LPTP)) 
      {
        // Display PBID and return Status.

        If(R30H)
        {
          Return(0x000F)
        }

        Return(0x000D)
      }
    }

    Return(0x0000)
  }

  // Disable Method for LPT-BiDirectional Mode.

  Method(_DIS,0,Serialized)
  {
    // Set SIO to LPT.

    Store(1,R07H)

    If(LEqual(And(RF0H,0xE0),0x20))
    {
      Store(0,R30H)
    } 
  }
   
  // Current Resource Setting Method for LPT-BiDirectional Mode.

  Method(_CRS,0,Serialized)
  {
    // Create a buffer to return to OS.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x0378,0x0378,0x01,0x04)     
      IRQNoFlags(){7}
    })

    // Skip this sequence if the LPT Device is not set
    // to BiDirectional Mode.

    If(LEqual(And(RF0H,0xE0),0x20))
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)
  
      // Set SIO to LPT.

      Store(1,R07H)

      // Write IO and Length values into the Buffer.
    
      Store(R61H,IOL0)
      Store(R60H,IOH0)
      Store(IOL0,IOL1)
      Store(IOH0,IOH1)
      Store(4,LEN0)

      // Write the IRQ value into the Buffer.

      If(And(R70H,0x0F))
      {
        ShiftLeft(One,And(R70H,0x0F),IRQW)
      }
      Else
      {
        Store(Zero, IRQW)
      }
    }

    Return(BUF0) 
  }

  // Possible Resource Setting Method for LPT-BiDirectional Mode.

    Name(_PRS,ResourceTemplate()
  {
      StartDependentFn(0,2)
      {
        IO(Decode16,0x378,0x378,0x01,0x04)
        IRQNoFlags(){7}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x278,0x278,0x01,0x04)
        IRQNoFlags(){7}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3BC,0x3BC,0x01,0x04)
        IRQNoFlags(){7}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x378,0x378,0x01,0x04)
        IRQNoFlags(){5}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x278,0x278,0x01,0x04)
        IRQNoFlags(){5}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3BC,0x3BC,0x01,0x04)
        IRQNoFlags(){5}
      }

      EndDependentFn()
    })
    

  // Set Resource Setting Method for LPT-BiDirectional Mode.

  Method(_SRS,1,Serialized) 
  {
    // Arg0 passed into _SRS is the buffer identical to BUF0.

    CreateByteField(Arg0,0x02,IOL0)
    CreateByteField(Arg0,0x03,IOH0)
    CreateWordField(Arg0,0x09,IRQW)

    Store(1,R07H)

    // Disable the LPT.

    Store(0,R30H)

    // Set to Bi-Directional Mode.

    Or(And(RF0H,0x0F),0x20,RF0H)

    // Disable the DMA channels.

            Store(4,R74H)
    
    // Set the Base IO Address.
    
            Store(IOL0,R61H)
            Store(IOH0,R60H)

    // Set the IRQ.

    FindSetRightBit(IRQW,Local0)
    If (LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,R70H)

    // Set the Decode Range so PBID works when docked.

    And(IOD1,0xFC,IOD1)     // Decode all bits.

    If(LEqual(IOH0,0x03))     // Address = 0x3xx
    {
      If(LEqual(IOL0,0x78))   // Address = 0x378
      {
        Or(IOD1,0x00,IOD1)
      }
      Else
      {
        Or(IOD1,0x02,IOD1)  // Address = 0x3BC
      }
    }
    Else          // Address = 0x278
    {
      Or(IOD1,0x01,IOD1)
    }

    // Enable the LPT.

    Store(1,R30H)
  }

  // D0 Method for LPT-BiDirectional Mode.

  Method(_PS0,0,Serialized)
  {
    Store(1,R07H)
    Store(1,R30H)
  }

  // D3 Method for LPT-BiDirectional Mode.

  Method(_PS3,0,Serialized)
  {
    Store(1,R07H)
    Store(0,R30H)
      }
}

Device(PEP1)  // Printer, EPP Mode
{
  Name(_HID,EISAID("PNP0400"))
  Name(_UID,3)

  // Status Method for LPT-EPP Mode.

  Method(_STA,0,Serialized) 
  {
    // EPP 1.9 mode = 011xxxxxb.

    Store(1,R07H)
    If(LOr(LEqual(And(RF0H,0xE0),0x60), LEqual(And(RF0H,0xE0),0x40)))
    {
      // LPT is in EPP Mode.  Only report resources
      // to the OS if the SIO Device is not set to
      // Disabled in BIOS Setup.

      If(LAnd(NATP,LPTP)) 
      {
        // Display PEPP and return Status.

        If(R30H)
        {
          Return(0x000F)
        }
        Else
        {
          Return(0x000D)
        }
      }
    }

    Return(0x0000)
  }

  // Disable Method for LPT-EPP Mode.

  Method(_DIS,0,Serialized)
  {
    // Set SIO to LPT.

    Store(1,R07H)

    If(LOr(LEqual(And(RF0H,0xE0),0x60), LEqual(And(RF0H,0xE0),0x40)))
    {
      Store(0,R30H)
    } 
  }
   
  // Current Resource Setting Method for LPT-EPP Mode.

  Method(_CRS,0,Serialized)
  {
    // Create a buffer to return to OS.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x0378,0x0378,0x01,0x08)     
      IRQNoFlags(){7}
    })

    // Skip this sequence if the LPT Device is not set
    // to EPP Mode.

    If(LOr(LEqual(And(RF0H,0xE0),0x60), LEqual(And(RF0H,0xE0),0x40)))
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)
  
      // Set SIO to LPT.

      Store(1,R07H)

      // Write IO and Length values into the Buffer.
    
      Store(R61H,IOL0)
      Store(R60H,IOH0)
      Store(IOL0,IOL1)
      Store(IOH0,IOH1)
      Store(8,LEN0)

      // Write the IRQ value into the Buffer.

      If(And(R70H,0x0F))
      {
        ShiftLeft(One,And(R70H,0x0F),IRQW)
      }
      Else
      {
        Store(Zero, IRQW)
      }
    }

    Return(BUF0) 
  }

  // Possible Resource Setting Method for LPT-EPP Mode.

    Name(_PRS,ResourceTemplate()
  {
      StartDependentFn(0,2)
      {
        IO(Decode16,0x378,0x378,0x01,0x08)
        IRQNoFlags(){7}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x278,0x278,0x01,0x08)
        IRQNoFlags(){7}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x378,0x378,0x01,0x08)
        IRQNoFlags(){5}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x278,0x278,0x01,0x08)
        IRQNoFlags(){5}
      }

      EndDependentFn()
    })
    

  // Set Resource Setting Method for LPT-EPP Mode.

  Method(_SRS,1,Serialized) 
  {
    // Arg0 passed into _SRS is the buffer identical to BUF0.

    CreateByteField(Arg0,0x02,IOL0)
    CreateByteField(Arg0,0x03,IOH0)
    CreateWordField(Arg0,0x09,IRQW)

    Store(1,R07H)

    // Disable the LPT.

    Store(0,R30H)

    // Set to EPP 1.9 Mode.

    Or(And(RF0H,0x0F),0x60,RF0H)

    // Disable the DMA channels.

            Store(4,R74H)
    
    // Set the Base IO Address.
    
            Store(IOL0,R61H)
            Store(IOH0,R60H)

    // Set the IRQ.

    FindSetRightBit(IRQW,Local0)
    If (LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,R70H)

    // Set the Decode Range so PEPP works when docked.

    And(IOD1,0xFC,IOD1)     // Decode all bits.

    If(LEqual(IOH0,0x03))     // Address = 0x378
    {
      Or(IOD1,0x00,IOD1)
    }
    Else          // Address = 0x278
    {
      Or(IOD1,0x01,IOD1)
    }

    // Enable the LPT.

    Store(1,R30H)
  }

  // D0 Method for LPT-EPP Mode.

  Method(_PS0,0,Serialized)
  {
    Store(1,R07H)
    Store(1,R30H)
  }

  // D3 Method for LPT-EPP Mode.

  Method(_PS3,0,Serialized)
  {
    Store(1,R07H)
    Store(0,R30H)
      }
}

Device(PEC1)  // Printer, ECP Mode
{
  Name(_HID,EISAID("PNP0401"))
  Name(_UID,4)

  // Status Method for LPT-ECP Mode.

  Method(_STA,0,Serialized) 
  {
    // ECP mode = 1111xxxxxb.

    Store(1,R07H)
    If(LOr(LEqual(And(RF0H,0xE0),0xE0), LEqual(And(RF0H,0xE0),0x80)))
    {
      // LPT is in ECP Mode.  Only report resources
      // to the OS if the SIO Device is not set to
      // Disabled in BIOS Setup.

      If(LAnd(NATP,LPTP)) 
      {
        // Display PECP and return Status.

        If(R30H)
        {
          Return(0x000F)
        }
        Else
        {
          Return(0x000D)
        }
      }
    }

    Return(0x0000)
  }

  // Disable Method for LPT-ECP Mode.

  Method(_DIS,0,Serialized)
  {
    // Set SIO to LPT.

    Store(1,R07H)

    If(LOr(LEqual(And(RF0H,0xE0),0xE0), LEqual(And(RF0H,0xE0),0x80)))
    {
      Store(0,R30H)
    } 
  }
   
  // Current Resource Setting Method for LPT-ECP Mode.

  Method(_CRS,0,Serialized)
  {
    // Create a buffer to return to OS.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x0378,0x0378,0x01,0x08)     
      IO(Decode16,0x0778,0x0778,0x01,0x08)
      IRQNoFlags(){7}
      DMA(Compatibility,NotBusMaster,Transfer8_16,){1}
    })

    // Skip this sequence if the LPT Device is not set
    // to ECP Mode.

    If(LOr(LEqual(And(RF0H,0xE0),0xE0), LEqual(And(RF0H,0xE0),0x80)))
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateByteField(BUF0,0x0A,IOL2)
      CreateByteField(BUF0,0x0B,IOH2)
      CreateByteField(BUF0,0x0C,IOL3)
      CreateByteField(BUF0,0x0D,IOH3)
      CreateByteField(BUF0,0x0F,LEN1)
      CreateWordField(BUF0,0x11,IRQW)
      CreateByteField(BUF0,0x14,DMA0)
  
      // Set SIO to LPT.

      Store(1,R07H)

      // Write IO and Length values into the Buffer.
    
      Store(R61H,IOL0)
      Store(R60H,IOH0)
      Store(IOL0,IOL1)
      Store(IOH0,IOH1)
      Store(IOL0,IOL2)
      Store(Add(0x4,IOH0),IOH2)
      Store(IOL0,IOL3)
      Store(Add(0x4,IOH0),IOH3)
      Store(8,LEN0)
      Store(8,LEN1)

      // Write the IRQ value into the Buffer.

      And(R70H,0x0F,Local0)
      If(Local0)
      {
        ShiftLeft(One,Local0,IRQW)
      }
      Else
      {
        Store(Zero,IRQW)
      }

      // Write the DMA value into the Buffer.

      Store(R74H,Local0)
      If(LEqual(Local0,0x04))
      {
        Store(Zero,DMA0)
      }
      Else
      {
        ShiftLeft(One,Local0,DMA0)
      }
    }

    Return(BUF0) 
  }

  // Possible Resource Setting Method for LPT-ECP Mode.

    Name(_PRS,ResourceTemplate()
  {
      StartDependentFn(0,2)
      {
        IO(Decode16,0x378,0x378,0x01,0x08)
        IO(Decode16,0x778,0x778,0x01,0x08)
        IRQNoFlags(){7}
        DMA(Compatibility,NotBusMaster,Transfer8_16){3}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x278,0x278,0x01,0x08)
        IO(Decode16,0x678,0x678,0x01,0x08)
        IRQNoFlags(){7}
        DMA(Compatibility,NotBusMaster,Transfer8_16){1,2,3}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x378,0x378,0x01,0x08)
        IO(Decode16,0x778,0x778,0x01,0x08)
        IRQNoFlags(){5}
        DMA(Compatibility,NotBusMaster,Transfer8_16){1,2,3}





      }

      EndDependentFn()
    })
    

  // Set Resource Setting Method for LPT-ECP Mode.

  Method(_SRS,1,Serialized) 
  {
    // Arg0 passed into _SRS is the buffer identical to BUF0.

    CreateByteField(Arg0,0x02,IOL0)
    CreateByteField(Arg0,0x03,IOH0)
    CreateWordField(Arg0,0x11,IRQW)
    CreateByteField(Arg0,0x14,DMA0)

    Store(1,R07H)

    // Disable the LPT.

    Store(0,R30H)

    // Set to ECP Mode with EPP Mode 4 support.

    Or(RF0H,0xF0,RF0H)

    // Set the Base I/O Address

            Store(IOL0,R61H)
            Store(IOH0,R60H)

    // Set the IRQ

    FindSetRightBit(IRQW,Local0)
    If (LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,R70H)

    // Set the DMA Channel

    FindSetRightBit(DMA0,Local0)
    If (LNotEqual(DMA0,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,R74H)

    // Set the Decode Range so PECP works when docked.

    And(IOD1,0xFC,IOD1)     // Decode all bits.

    If(LEqual(IOH0,0x03))     // Address = 0x378
    {
      Or(IOD1,0x00,IOD1)
    }
    Else          // Address = 0x278
    {
      Or(IOD1,0x01,IOD1)
    }

    // Enable the LPT.

    Store(1,R30H)
  }

  // D0 Method for LPT-ECP Mode.

  Method(_PS0,0,Serialized)
  {
    Store(1,R07H)
    Store(1,R30H)
  }

  // D3 Method for LPT-ECP Mode.

  Method(_PS3,0,Serialized)
  {
    Store(1,R07H)
    Store(0,R30H)
      }
}
