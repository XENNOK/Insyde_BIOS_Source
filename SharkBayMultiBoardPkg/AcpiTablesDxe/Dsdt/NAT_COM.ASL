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



Device(UAR3)  // Serial Port A
{
  Name(_HID, EISAID("PNP0501"))

  Name(_UID,2)

  // Status Method for COM A.

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // not set to Disabled in BIOS Setup.

    If(LAnd(NATP,CMAP))
    {
      // Set the SIO to COM A.

      Store(3,R07H)

      // Display COMA and return status.

      If(R30H) 
      {
        Return(0x000F)
      }

      Return(0x000D)
    }

    Return(0x0000)
  }

  // Disable Method for COM A.

  Method(_DIS,0,Serialized)
  {
    Store(3,R07H)
    Store(0,R30H)
  }

  // Current Resource Setting Method for COM A.

  Method(_CRS,0,Serialized)
  {
    // Create the Buffer that stores the Resources to
    // be returned.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x03F8,0x03F8,0x01,0x08)
      IRQNoFlags(){4}
    })

    // Set SIO to COM A.

    Store(3,R07H)

    // Skip this sequence if the COMA Port is Disabled
    // in BIOS Setup.

    If(LAnd(NATP,CMAP))
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)

      // Write IO and Length values into the Buffer.
    
      Store(R60H,IOH0)
      Store(R61H,IOL0)
      Store(R60H,IOH1)
      Store(R61H,IOL1)
      Store(8,LEN0)

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
    }

    Return(BUF0)
  }

  // Possible Resource Setting Method for COM A.

    // Build a Buffer with all valid COM Port Resources.

    Name(_PRS,ResourceTemplate()
    {
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2E8,0x2E8,0x01,0x08) 
        IRQNoFlags(){3}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3F8,0x3F8,0x01,0x08) 
        IRQNoFlags(){3,4,5,6,7,10,11,12} 
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2F8,0x2F8,0x01,0x08) 
        IRQNoFlags(){3,4,5,6,7,10,11,12} 
      }
    
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3E8,0x3E8,0x01,0x08) 
        IRQNoFlags(){3,4,5,6,7,10,11,12} 
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2E8,0x2E8,0x01,0x08) 
        IRQNoFlags(){3,4,5,6,7,10,11,12} 
      }
  
      EndDependentFn()
    })


  // Set Resource Setting Method for COM A.

  Method(_SRS,1,Serialized)
  {
    // Point to the specific information in the passed
    // in Buffer.

    CreateByteField(Arg0,0x02,IOLO)
    CreateByteField(Arg0,0x03,IOHI)
    CreateWordField(Arg0,0x09,IRQW)

    // Set the SIO to COM A.

    Store(3,R07H)

    // Disable the device.

    Store(0,R30H)

    // Set the Base IO Address.

            Store(IOLO,R61H)
            Store(IOHI,R60H)

    // Set the IRQ.

    FindSetRightBit(IRQW,Local0)
    If(LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,R70H)

    // Set the Decode Range so COM A works on ICH7 and
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

    Store(1,R30H)
  }

  // D0 Method for COM Port.

  Method(_PS0,0,Serialized)
  {
    Store(3,R07H)
    Store(1,R30H)
  }

  // D3 Method for COM Port.

  Method(_PS3,0,Serialized)
  {
    Store(3,R07H)
    Store(0,R30H)
      }
}

Device(UAR4)  // Serial Port B
{
  Name(_HID, EISAID("PNP0501"))

  Name(_UID,3)

  // Status Method for COM B.

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // not set to Disabled in BIOS Setup.

    If(LAnd(NATP,CMBP))
    {
      // Set the SIO to COM B.

      Store(2,R07H)

      // Display COM B and return status.

      If(R30H) 
      {
        Return(0x000F)
      }

      Return(0x000D)
    }

    Return(0x0000)
  }

  // Disable Method for COM B.

  Method(_DIS,0,Serialized)
  {
    Store(2,R07H)
    Store(0,R30H)
  }

  // Current Resource Setting Method for COM B.

  Method(_CRS,0,Serialized)
  {
    // Create the Buffer that stores the Resources to
    // be returned.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x02F8,0x02F8,0x01,0x08)
      IRQNoFlags(){3}
    })

    // Set SIO to COM B.

    Store(2,R07H)

    // Skip this sequence if the COM B Port is Disabled
    // in BIOS Setup.

    If(LAnd(NATP,CMBP))
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)

      // Write IO and Length values into the Buffer.
    
      Store(R60H,IOH0)
      Store(R61H,IOL0)
      Store(R60H,IOH1)
      Store(R61H,IOL1)
      Store(8,LEN0)

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
    }

    Return(BUF0)
  }

  // Possible Resource Setting Method for COM B.

    // Build a Buffer with all valid COM B Port Resources.

    Name(_PRS,ResourceTemplate()
    {
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2E8,0x2E8,0x01,0x08) 
        IRQNoFlags(){3}
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2F8,0x2F8,0x01,0x08) 
        IRQNoFlags(){3}
      }
    
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3E8,0x3E8,0x01,0x08) 
        IRQNoFlags(){3}
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3F8,0x3F8,0x01,0x08) 
        IRQNoFlags(){3}
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2E8,0x2E8,0x01,0x08) 
        IRQNoFlags(){4}
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2F8,0x2F8,0x01,0x08) 
        IRQNoFlags(){4}
      }
    
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3E8,0x3E8,0x01,0x08) 
        IRQNoFlags(){4}
      }
  
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3F8,0x3F8,0x01,0x08) 
        IRQNoFlags(){4}
      }
  
      EndDependentFn()
    })


  // Set Resource Setting Method for COM B.

  Method(_SRS,1,Serialized)
  {
    // Point to the specific information in the passed
    // in Buffer.

    CreateByteField(Arg0,0x02,IOLO)
    CreateByteField(Arg0,0x03,IOHI)
    CreateWordField(Arg0,0x09,IRQW)

    // Set the SIO to COM B.

    Store(2,R07H)

    // Disable the device.

    Store(0,R30H)

    // Set the Base IO Address.

            Store(IOLO,R61H)
            Store(IOHI,R60H)

    // Set the IRQ.

    FindSetRightBit(IRQW,Local0)
    If(LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,R70H)

    // Set the Decode Range so COM B works on ICH7 and
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

    Store(1,R30H)
  }

  // D0 Method for COM B Port.

  Method(_PS0,0,Serialized)
  {
    Store(2,R07H)
    Store(1,R30H)
  }

  // D3 Method for COM B Port.

  Method(_PS3,0,Serialized)
  {
    Store(2,R07H)
    Store(0,R30H)
      }
}
