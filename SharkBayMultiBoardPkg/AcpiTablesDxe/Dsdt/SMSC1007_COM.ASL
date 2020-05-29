//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
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


Device(UAR1)  // SMSC 1007 UART 1.
{
  // Identify UART 2 as a COM Port.

  Name(_HID, EISAID("PNP0501"))

  Name(_UID,0)
  
  // Enable Wake on Ring through a Serial Modem.

//[-start-121128-IB06460471-modify]//
//[-start-130513-IB06150294-modify]//
#ifdef ULT_SUPPORT
//[-end-130513-IB06150294-modify]//
  Method(_PRW, 0) { Return(GPRW(0x68, 3)) } // can wakeup from S3 state
#else
  Method(_PRW, 0) { Return(GPRW(0x08, 3)) } // can wakeup from S3 state
#endif
//[-end-121128-IB06460471-modify]//
  
  // Status Method for UART 1.

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // not set to Disabled in BIOS Setup.

//[-start-120921-IB10370021-modify]//
    If(LAnd(SMSC,CMCP))
//[-end-120921-IB10370021-modify]//
    {
      // Display UART 1 and return status.

      Store(0,CR55)
      Store(CR02,Local0)
      Store(0,CRAA)

      If(And(Local0,0x08))
      {
        Return(0x000F)
      }

      Return(0x000D)
    }

    Return(0x0000)
  }

  // Disable Method for UART 1.

  Method(_DIS,0,Serialized)
  {
    Store(0,CR55)
    Store(And(CR02,0xF0),CR02)
    Store(0,CRAA)
  }

  // Current Resource Setting Method for UART 1.

  Method(_CRS,0,Serialized)
  {
    // Create the Buffer that stores the Resources to
    // be returned.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x03F8,0x03F8,0x01,0x08)
      IRQNoFlags(){4}
    })

    // Skip this sequence if UART 1 is Disabled
    // in BIOS Setup.

//[-start-120921-IB10370021-modify]//
    If(LAnd(SMSC,CMCP))
//[-end-120921-IB10370021-modify]//
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)

      // Write the base address into the Buffer.

      Store(0,CR55)

      ShiftLeft(CR24,2,IOL0)    // Low Byte.
      ShiftLeft(CR24,2,IOL1)

      ShiftRight(CR24,6,IOH0)   // High Byte.
      ShiftRight(CR24,6,IOH1)

      // Write the length into the Buffer.

      Store(8,LEN0)

      // Write the IRQ value into the Buffer.

      ShiftLeft(One,ShiftRight(And(CR28,0xF0),4),IRQW)

      Store(0,CRAA)
    }

    Return(BUF0)
  }

  // Possible Resource Setting Method for UART 1.


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


  // Set Resource Setting Method for UART 1.

  Method(_SRS,1,Serialized)
  {
    // Point to the specific information in the passed
    // in Buffer.

    CreateByteField(Arg0,0x02,IOLO)
    CreateByteField(Arg0,0x03,IOHI)
    CreateWordField(Arg0,0x09,IRQW)

    // Disable the device.

    Store(0,CR55)
    Store(And(CR02,0xF0),CR02)

    // Set the Base IO Address.

            Store(ShiftRight(IOLO,2),CR24)
            Or(CR24,ShiftLeft(IOHI,6),CR24)

    // Set the IRQ.

    And(CR28,0x0F,CR28)
    Or(CR28,ShiftLeft(Subtract(FindSetRightBit(IRQW),1),4),CR28)

    // Set the Decode Range so UART 1 works with ICH7 and
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

    Store(Or(CR02,0x08),CR02)
    Store(0,CRAA)
  }

  // D0 Method for UART 1.

  Method(_PS0,0,Serialized)
  {
    Store(0,CR55)
    Store(Or(CR02,0x08),CR02)
    Store(0,CRAA)
  }

  // D3 Method for UART 1.

  Method(_PS3,0,Serialized)
  {
    Store(0,CR55)
    Store(And(CR02,0xF0),CR02)
    Store(0,CRAA)
  }
}

Device(UAR2)  // SMSC 1007 UART 2.
{
  // Identify UART 2 as an IR Device.

  Name(_HID, EISAID("PNP0510"))

  Name(_UID,0)

  // Status Method for UART 2.

  Method(_STA,0,Serialized)
  {
    //
    //Check SIR Base Address, If ADR9 and ADR8 are 0, SIR disabled. 
    //
//[-start-120921-IB10370021-modify]//
    If (LAnd(SMSC,CIRP))
//[-end-120921-IB10370021-modify]//
    {
      Store(0,CR55)
            Store(CR02,Local0)
            Store(0,CRAA)    
            If (And(Local0,0x80))
            {
              Return (0x000F)
            }
            Return (0x000D)     // Not present
      }
      Return (0x0000)
  }
  // Disable Method for UART 2.

  Method(_DIS,0,Serialized)
  {
    Store(0,CR55)
    
    Store(And(CR02,0x0F),CR02)

      //
      // Disable IR on Logical Device Activate Register
      //
    Store(And(CR3A,0xFA),CR02)
                  
    Store(0,CRAA)
  }


    //
  // Current Resource Setting Method for UART 2.
    //
  Method(_CRS,0,Serialized)
  {

        // Create the FIR Buffer that stores the Resources to
        // be returned.
        Name(BUF0,ResourceTemplate()
        {
            IO(Decode16, 0x0000, 0x0000, 0x01, 0x08)
            IO(Decode16, 0x0000, 0x0000, 0x01, 0x08)
            IRQNoFlags() {7}
            DMA(Compatibility, NotBusMaster, Transfer8, ) {2}
          })
    
    // Skip this sequence if UART 2 is Disabled
    // in BIOS Setup.

//[-start-120921-IB10370021-modify]//
    If (LAnd(SMSC,CIRP))
//[-end-120921-IB10370021-modify]//
    {
      //
            // Create pointers to the specific byte.
                  //
            CreateByteField(BUF0,0x02,IO10)
            CreateByteField(BUF0,0x03,IO11)
            CreateByteField(BUF0,0x04,IO12)
            CreateByteField(BUF0,0x05,IO13)
            CreateByteField(BUF0,0x0a,IO20)
            CreateByteField(BUF0,0x0b,IO21)
            CreateByteField(BUF0,0x0c,IO22)
            CreateByteField(BUF0,0x0d,IO23)
                CreateWordField(BUF0,0x11,IRQW)
        CreateByteField(BUF0,0x14,DMA0)

      Store(0,CR55)
      
      //
      // Write the Base Address into the Buffer.
      //
      ShiftLeft(CR25,2,IO10)    // Low Byte.
      ShiftRight(And(CR25,0xC0), 6, IO11)
      Store(IO10, IO12)
      Store(IO11, IO13)
      
      ShiftLeft(CR2B,3,IO20)    // Low Byte.
      ShiftRight(And(CR2B,0xE0), 5, IO21)
      Store(IO20, IO22)
      Store(IO21, IO23)
      
      //
      // Write the IRQ value into the Buffer.
      //
      Store(0x0,Local1)
      And(CR28,0x0F,Local1)
      ShiftLeft(1, Local1, IRQW)
      
      // Write DMA value into buffer
      Store(0x0,Local1)
      And(CR2C,0x0F,Local1)
      If(LEqual(Local1, 0x0F)){
        Store(0, DMA0)  //No DMA
      }Else{
        ShiftLeft(1, Local1, DMA0)
      }
      
      Store(0,CRAA)
    }

        Return(BUF0)
        }

  //
  // Possible Resource Setting Method for UART 2.
  //
    // Build a Buffer with all valid UART 2 Resources.

  Name(_PRS, ResourceTemplate() {
// IRQ, No DMA
StartDependentFn(0, 0) {
        IO(Decode16,0x3E8,0x3E8,0x01,0x08) 
      IO(Decode16,0x7E8,0x7E8,0x01,0x08)
        IRQNoFlags(){3}
      DMA(Compatibility, NotBusMaster, Transfer8_16) {3}
    }
    StartDependentFnNoPri() {
      IO(Decode16, 0x3F8, 0x3F8, 1, 8)
      IO(Decode16,0x7E8,0x7E8,0x01,0x08)
      IRQNoFlags() {3,4,5,6,7,10,11,12}
      DMA(Compatibility, NotBusMaster, Transfer8_16) {1,2,3}
    }
    StartDependentFnNoPri() {
      IO(Decode16, 0x2F8, 0x3F8, 1, 8)
      IO(Decode16,0x7E8,0x7E8,0x01,0x08)
      IRQNoFlags() {3,4,5,6,7,10,11,12}
      DMA(Compatibility, NotBusMaster, Transfer8_16) {1,2,3}
    }
    StartDependentFnNoPri() {
      IO(Decode16, 0x3E8, 0x3E8, 1, 8)
      IO(Decode16,0x7E8,0x7E8,0x01,0x08)
      IRQNoFlags() {3,4,5,6,7,10,11,12}
      DMA(Compatibility, NotBusMaster, Transfer8_16) {1,2,3}
    }
    StartDependentFnNoPri() {
      IO(Decode16, 0x2E8, 0x3E8, 1, 8)
      IO(Decode16,0x7E8,0x7E8,0x01,0x08)
      IRQNoFlags() {3,4,5,6,7,10,11,12}
      DMA(Compatibility, NotBusMaster, Transfer8_16) {1,2,3}
    }
    EndDependentFn()

  })


  // Set Resource Setting Method for UART 2.
  // Set Resource Setting Method for UART 2.
  Method(_SRS,1,Serialized)
  {
    Store(0,CR55)
    Store(CR2B, Local0)
    Store(0,CRAA)
    If(Local0)
        {
        // Point to the specific information in the passed
        // in Buffer.
          CreateByteField(Arg0,0x02,IO10)
          CreateByteField(Arg0,0x03,IO11)
          CreateByteField(Arg0,0x0a,IO20)
          CreateByteField(Arg0,0x0b,IO21)
            CreateWordField(Arg0,0x11,IRQW)
        CreateByteField(Arg0,0x14,DMA0)

        Store(0,CR55)

            //
        // Disable the device.
            //
        And(CR02,0x0F,CR02)

            // 
            // If UART2 mode has not already been configured, set it here.
            //
//        Store(0x0B, CR0C)   
//        Store(0x40, CR0A)   
//        Store(0x0C, CR0C)   

            //
        // Set the Base IO Address.
            //
            Store(ShiftRight(IO10,2),CR25)
            Or(CR25,ShiftLeft(IO11,6),CR25)
      If(IO20){
                Store(ShiftRight(IO20,3),CR2B)
                Or(CR2B,ShiftLeft(IO21,5),CR2B)
      }


            //
        // Set the IRQ.
            //
        And(CR28,0xF0,CR28)
        Or(CR28,Subtract(FindSetRightBit(IRQW),1),CR28)

            // Set DMA
            And(CR2C,0xF0,CR2C)
            If(DMA0)
            {
            Or(CR2C,Subtract(FindSetRightBit(DMA0),1),CR2C)
            }
            Else
            {
            Or(CR2C,0x0F,CR2C) // No DMA
            }

        //
        // Set the Decode Range so UART 2 works with ICH7 and
        // future platforms.
            //
        AND(IOD0,0x8F,IOD0)        // Clear all bits.
        If(LEqual(IO11,0x03))      // Address = 0x3xx
        {
          If(LEqual(IO10,0xF8))    // Address = 0x3F8
          {
            Or(IOD0,0x00,IOD0)
          }
          Else                     // Address = 0x3E8
          {
            Or(IOD0,0x070,IOD0)
          }
        }
        Else                       // Address = 0x2xx
        {
          If(LEqual(IO10,0xF8))    // Address = 0x2F8
          {
            Or(IOD0,0x010,IOD0) 
          }
          Else                     // Address = 0x2E8
          {
            Or(IOD0,0x050,IOD0)
          }
        }

            //
        // Enable the device.
            // 
        Or(CR02,0x80,CR02)
        
        Store(0,CRAA)

        }
        Else
        {
        // Point to the specific information in the passed
        // in Buffer.
        CreateByteField(Arg0,0x02,IOLO)
        CreateByteField(Arg0,0x03,IOHI)
        CreateWordField(Arg0,0x09,IRQR)

        Store(0,CR55)

            //
        // Disable the device.
            //
        And(CR02,0x0F,CR02)

            // 
            // If UART2 mode has not already been configured, set it here.
            //
//        Store(0x0B, CR0C)   
//        Store(0x40, CR0A)   
//        Store(0x0C, CR0C)   

            //
        // Set the Base IO Address.
            //
            Store(ShiftRight(IOLO,2),CR25)
            Or(CR25,ShiftLeft(IOHI,6),CR25)

            //
        // Set the IRQ.
            //
        And(CR28,0xF0,CR28)
        Or(CR28,Subtract(FindSetRightBit(IRQR),1),CR28)

        //
        // Set the Decode Range so UART 2 works with ICH7 and
        // future platforms.
            //
        AND(IOD0,0x8F,IOD0)        // Clear all bits.

        If(LEqual(IOHI,0x03))      // Address = 0x3xx
        {
          If(LEqual(IOLO,0xF8))    // Address = 0x3F8
          {
            Or(IOD0,0x00,IOD0)
          }
          Else                     // Address = 0x3E8
          {
            Or(IOD0,0x070,IOD0)
          }
        }
        Else                       // Address = 0x2xx
        {
          If(LEqual(IOLO,0xF8))    // Address = 0x2F8
          {
            Or(IOD0,0x010,IOD0) 
          }
          Else                     // Address = 0x2E8
          {
            Or(IOD0,0x050,IOD0)
          }
        }

            //
        // Enable the device.
            // 
        Or(CR02,0x80,CR02)
        
        Store(0,CRAA)
        }
    }

  //
  // D0 Method for UART 2.
  //
  Method(_PS0,0,Serialized)
  {
    Store(0,CR55)
    Store(Or(CR02,0x80),CR02)
    Store(0,CRAA)
    
  }

  //
  // D3 Method for UART 2.
  //
  Method(_PS3,0,Serialized)
  {
    Store(0,CR55)
    Store(And(CR02,0x0F),CR02)
    Store(0,CRAA)

  }
}
