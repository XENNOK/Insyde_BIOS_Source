/** @file

ASL code for SIO

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#define SIO_LPT    CLDN

Device (LPT) {  // Legacy printer port
    
  Name (_HID, EISAID("PNP0400"))  // PnP Device ID
  Name (_UID, "SModuleL")

  //*****************************************************
  // Method _STA:  Return Status
  //*****************************************************
  Method (_STA, 0, NotSerialized) { // Return Status of the LPT

    // Enter Configuration Mode
    ENCG()

    // Select LPT
    Store (SIO_LPT, CR07)

    //  First, read the Mode register and see if we're in LPT mode
    And(CRF0, 0x07, Local0) // extract mode bits from OPT0 register

    If (LNotEqual (Local0, 0x02)) {  // OK, we're not in ECP mode
      //
      // OK, we're not in ECP mode, so read the Mailbox register.
      // If this register is TRUE, then that means
      // that the device is Present & Active.
      //
      Store (0x00, Local0)      
     
      //
      // Depend on SCU value through MailBox to enable or disable this device
      // 
      if (CDVC) {             
        Store (0x0D, Local0)   

        //
        // Check enable register
        // local device is exist and enable
        //
        If (CR30) {
          Store (0x0F, Local0)  
        }
      }
      EXCG()
      Return (Local0)
    }
    
    Else {
      // Exit Configuration Mode
      EXCG()

      // Return result -- Device Not Present (better be ECP mode)
      Return (0x0)
    }
  } // end of Method _STA    


  //*****************************************************
  //  Method _DIS:  Disable the device
  //*****************************************************
  Method (_DIS, 0, NotSerialized) { // Disable Method
    // Enter Configuration Mode
    ENCG ()

    // Set Logical Device to select the LPT
    Store (SIO_LPT, CR07)

    // Set the Activate Register to zero to disable device
    Store (0x00, CR30)

    // Exit Configuration Mode
    EXCG ()
  }


  //*****************************************************
  //  Method _CRS:  Return Current Resource Settings
  //*****************************************************
  Method (_CRS, 0, NotSerialized) { 

    Name (BUF0, ResourceTemplate () { // Length of Buffer
      IO (Decode16, 0x378, 0x378, 0x08, 0x08)
      IRQNoFlags(){7}
    })
    //
    // Create some ByteFields in the Buffer in order to
    // permit saving values into the data portions of
    // each of the descriptors above.
    //
    CreateByteField (BUF0, 0x02, IOLO) // IO Port Low
    CreateByteField (BUF0, 0x03, IOHI) // IO Port Hi
    CreateByteField (BUF0, 0x04, IORL) // IO Port Low
    CreateByteField (BUF0, 0x05, IORH) // IO Port High
    CreateByteField (BUF0, 0x09, IRQL) // IRQ Low
 
    // Enter Configuration Mode
    ENCG()

    // Select the LPT
    Store (SIO_LPT, CR07)
 
    //
    // Get the IO setting from the chip, and copy it
    // to both the min & max IO descriptors
    //
    Store (CR61, IOLO)  // io, min.
    Store (CR61, IORL)  // io, max.
 
    Store (CR60, IOHI)   // io, min.
    Store (CR60, IORH)   // io, max.
 
    //
    // Get the IRQ setting from the chip, and shift
    // it into the Descriptor byte.  Better not be > 7!!!
    //
    ShiftLeft (0x01, CR70, IRQL)
 
    // Exit Configuration Mode
    EXCG()

    Return(BUF0) // return the result
  
  } // end _CRS Method

  //*****************************************************
  //  Method _PRS:  Return Possible Resource Settings
  //*****************************************************
  // Mechanism to return list of possible resource groupings. 
  // Start has two parameters. This first is the priority for legacy OSs.
  // The second is the priority for performance(ACPI) OSs.
  // Valid values are:
  //    Good        - Perferred resource grouping.
  //    Acceptable  - Acceptable resource grouping.
  //    SubOptimal  - Least perferred resource grouping.
  // Normally one parameter is Good while the other is subOptimal.

  Method (_PRS, 0) {  // LPT Possible Resources

    //--------- Create buffer with possible resources (PnP descriptor)
    Name(BUF0, ResourceTemplate() {  // ASL compiler will fill-in Buffer Length
    
      // Good configuration for Legacy O/S and sub-optimal for ACPI O/S
      StartDependentFn (0, 2) {
        IO (Decode16, 0x378, 0x378, 0x01, 0x08)
        IRQNoFlags(){7, 5}
      }

      // Good configuration for Legacy O/S and sub-optimal for ACPI O/S
      StartDependentFn (0, 2) { 
        IO (Decode16, 0x378, 0x378, 0x01, 0x08)
        IRQNoFlags(){5}
      }

      // Good configuration for Legacy O/S and sub-optimal for ACPI O/S
      StartDependentFn (0, 2) {
        IO (Decode16, 0x278, 0x278, 0x01, 0x08)
        IRQNoFlags(){7}
      }

      // Good configuration for Legacy O/S and sub-optimal for ACPI O/S
      StartDependentFn (0, 2) {
        IO (Decode16, 0x278, 0x278, 0x01, 0x08)
        IRQNoFlags(){5}
      }

      // Sub-optimal configuration for Legacy O/S and good for ACPI O/S
      StartDependentFn (0, 2) {
        IO (Decode16, 0x100, 0x3FC, 0x04, 0x04) 
        IRQNoFlags() {15, 14, 13, 12, 11, 10, 8, 7, 6, 5, 4, 3, 1}
      }
      EndDependentFn()
    })
    Return(BUF0)  // Return BUF0 
  }  // End _PRS method
  

  //*****************************************************
  //  Method _SRS:  Set Resource Setting
  //*****************************************************
  Method(_SRS,1,NotSerialized) {    // Set Resource Settings
    // ARG0 is PnP Resource String to set

    //
    // Define offsets into descriptor to match value
    // returned by descriptors in _PRS method
    //
    CreateByteField (ARG0, 0x02, IOLO) // IO Port Low
    CreateByteField (ARG0, 0x03, IOHI) // IO Port Hi
    CreateWordField (Arg0, 0x02, IOAD) // IO Word
    CreateByteField (ARG0, 0x09, IRQL) // IRQ

    // Enter Configuration Mode
    ENCG ()

    // Set Logical Device to select LPT
    Store (SIO_LPT, CR07)

    // Set the IO Base Address
    Store (IOLO, CR61)
    Store (IOHI, CR60)

    // Set IRQ
    FindSetRightBit (IRQL, CR70)
    If (LNotEqual (IRQL, Zero)) {
      Decrement(CR70)
    }

    // Activate the Device
    Store (0x01, CR30)

    // Exit Configuration Mode
    EXCG ()
    
  } // end _SRS Method
} // End LPT Device



//***********************************************************************//

Device (ECP) { // ECP printer port

  Name (_HID, EISAID("PNP0401"))  // PnP Device ID


  //*****************************************************
  // Method _STA:  Return Status
  //*****************************************************
  Method(_STA, 0, NotSerialized) { // Return Status of the ECP

    // Enter Configuration Mode
    ENCG ()

    // Select ECP
    Store (SIO_LPT,CR07)

    //  First, read the Mode register and see if we're in ECP mode
    And(CRF0, 0x07, Local0) // extract mode bits from OPT0 register

    If (LEqual (Local0, 0x02)) {  // OK, we're in ECP mode
      //
      // OK, we're in ECP mode, so read the Activate register.
      // If this register is TRUE, then that means
      // that the device is Present & Active.
      //
      If (CR30) {
        // Exit Configuration Mode
        EXCG ()

        // Return result --
        //      Device is Present & Active
        //      did *NOT* fail self-test
        //      SHOULD be shown to user
        Return(0x0F)
      }

      //
      // OK, so the device isn't Active, so check
      // and see if it's at least Present.
      //
      Else {
        //
        // If the I/O Address is non-zero, then the device
        // is Present.  Test for zero by OR'ing together
        // the high and low bytes of the address.
        //
        If (LOr (CR60, CR61)) {
          // Exit Configuration Mode
          EXCG ()

          //
          // Return result --
          //      Device is Present, but not Active
          //      did *NOT* fail self-test
          //      SHOULD be shown to user
          //
          Return(0x0D)
        }
        Else {
          // Exit Configuration Mode
          EXCG ()

          // Return result -- Device Not Present
          Return(0x0)  // code = Device not even Present
        }
      }
    }
    Else {
      // Exit Configuration Mode
      EXCG ()

      // Return result -- Device Not Present (better be LPT mode)
      Return (0x0)
    }

  } // end of Method _STA



  //*****************************************************
  //  Method _DIS:  Disable the device
  //*****************************************************
  Method(_DIS, 0, NotSerialized) { // Disable Method
    // Enter Configuration Mode
    ENCG ()

    // Set Logical Device  to select the ECP
    Store (SIO_LPT, CR07)

    // Set the Activate Register to zero to disable device
    Store (0x0, CR30)

    // Exit Configuration Mode
    EXCG ()
  }


  //*****************************************************
  //  Method _CRS:  Return Current Resource Settings
  //*****************************************************
  Method (_CRS, 0, NotSerialized) {
  
    Name (BUF0, ResourceTemplate() {
      IO (Decode16, 0x378, 0x378, 0x08, 0x08)
      IRQNoFlags(){7}
    })

    //
    // Create some ByteFields in the Buffer in order to
    // permit saving values into the data portions of
    // each of the descriptors above.
    //
    CreateByteField (BUF0, 0x02, IOLO) // IO Port Low
    CreateByteField (BUF0, 0x03, IOHI) // IO Port Hi
    CreateByteField (BUF0, 0x04, IORL) // IO Port Low
    CreateByteField (BUF0, 0x05, IORH) // IO Port High
    CreateByteField (BUF0, 0x0A, IULO) // Upper IO Port Low
    CreateByteField (BUF0, 0x0B, IUHI) // Upper IO Port Hi
    CreateByteField (BUF0, 0x0C, IURL) // Upper IO Port Low
    CreateByteField (BUF0, 0x0D, IURH) // Upper IO Port High
    CreateByteField (BUF0, 0x11, IRQL) // IRQ Low
    CreateByteField (BUF0, 0x14, DMAV) // DMA

    // Enter Configuration Mode
    ENCG ()

    // Select the ECP
    Store (SIO_LPT, CR07)

    //
    // Get the IO setting from the chip, and copy it
    // to both the min & max IO descriptors
    //
    Store (CR61, IOLO)  // io, min.
    Store (CR61, IORL)  // io, max.

    Store (CR60, IOHI)   // io, min.
    Store  (CR60, IORH)   // io, max.

    Store (CR61, IULO)               // Upper IO, min.
    Store (CR61, IURL)               // Upper IO, max.

    Add (CR60, 0x04, IUHI)           // Upper IO, min.
    Add (CR60, 0x04, IURH)           // Upper IO, max.

    //
    // Get the IRQ setting from the chip, and shift
    // it into the Descriptor byte.
    //
    Store (0x01,Local0)
    ShiftLeft (Local0, CR70, IRQL)

    //
    // Get the DMA setting from the chip.
    // and shift it into the Descriptor Byte.
    //
    If (Lequal (CR74, 0x04)) {
      // Register value of 4 indicates disable
      Store (Zero, Local0)
    } Else {
      Store (One, Local0)
    }

    ShiftLeft (Local0, CR74, DMAV)

    // Exit Configuration Mode
    EXCG ()

    Return (BUF0) // return the result

  } // end _CRS Method


  //*****************************************************
  //  Method _PRS:  Return Possible Resource Settings
  //*****************************************************
  // Mechanism to return list of possible resource groupings.
  // Start has two parameters. This first is the priority for legacy OSs.
  // The second is the priority for performance(ACPI) OSs.
  // Valid values are:
  //    Good        - Perferred resource grouping.
  //    Acceptable  - Acceptable resource grouping.
  //    SubOptimal  - Least perferred resource grouping.
  // Normally one parameter is Good while the other is subOptimal.
  Method (_PRS, 0) {        // ECP Possible Resources

    //--------- Create buffer with possible resources (PnP descriptor)  
    Name (BUF0, ResourceTemplate() {
    
      // Good configuration for Legacy O/S and good for ACPI O/S
      StartDependentFn (0, 0) {
        IO (Decode16, 0x378, 0x378, 0x01, 0x08)
        IRQNoFlags(){7}
      }

      // Good configuration for Legacy O/S and good for ACPI O/S
      StartDependentFn (0, 0) {
        IO (Decode16, 0x378, 0x378, 0x01, 0x08)
        IO (Decode16, 0x778, 0x778, 0x01, 0x08)
        IRQNoFlags(){7}
        DMA (Compatibility, NotBusMaster, Transfer8){1}
      }

      // Good configuration for Legacy O/S and good for ACPI O/S
      StartDependentFn (0, 0) {
        IO (Decode16, 0x378, 0x378, 0x01, 0x08)
        IO (Decode16, 0x778, 0x778, 0x01, 0x08)
        IRQNoFlags(){5}
        DMA (Compatibility, NotBusMaster, Transfer8){3}
      }

      // Good configuration for Legacy O/S and good for ACPI O/S
      StartDependentFn (0, 0) {
        IO (Decode16, 0x378, 0x378, 0x01, 0x08)
        IO (Decode16, 0x778, 0x778, 0x01, 0x08)
        IRQNoFlags(){5}
        DMA (Compatibility, NotBusMaster, Transfer8){1}
      }

      // Good configuration for Legacy O/S and good for ACPI O/S
      StartDependentFn (0, 0) {
        IO (Decode16, 0x278, 0x278, 0x01, 0x08)
        IO (Decode16, 0x678, 0x678, 0x01, 0x08)
        IRQNoFlags(){7}
        DMA (Compatibility, NotBusMaster, Transfer8){3}
      }

      // Good configuration for Legacy O/S and good for ACPI O/S
      StartDependentFn (0, 0) {
         IO (Decode16, 0x278, 0x278, 0x01, 0x08)
         IO (Decode16, 0x678, 0x678, 0x01, 0x08)
         IRQNoFlags(){7}
         DMA (Compatibility, NotBusMaster, Transfer8){1}
      }

      // Good configuration for Legacy O/S and good for ACPI O/S
      StartDependentFn (0, 0) {
         IO (Decode16, 0x278, 0x278, 0x01, 0x08)
         IO (Decode16, 0x678, 0x678, 0x01, 0x08)
         IRQNoFlags(){5}
         DMA (Compatibility, NotBusMaster, Transfer8){3}
      }

      // Good configuration for Legacy O/S and good for ACPI O/S
      StartDependentFn (0, 0) {
        IO (Decode16, 0x278, 0x278, 0x01, 0x08)
        IO (Decode16, 0x678, 0x678, 0x01, 0x08)
        IRQNoFlags(){5}
        DMA (Compatibility, NotBusMaster, Transfer8){1}
      }

      EndDependentFn()
    })
    Return(BUF0)
  }                 // End _PRS method


  //*****************************************************
  //  Method _SRS:  Set Resource Setting
  //*****************************************************
  Method(_SRS, 1, NotSerialized) {    // Set Resource Settings
    // ARG0 is PnP Resource String to set

    //
    // Define offsets into descriptor to match value
    // returned by descriptors in _CRS method
    //
    CreateByteField (ARG0, 0x02, IOLO) // IO Port Low
    CreateByteField (ARG0, 0x03, IOHI) // IO Port Hi
    CreateWordField (Arg0, 0x02, IOAD) // IO Word
    CreateByteField (ARG0, 0x11, IRQL) // IRQ
    CreateByteField (ARG0, 0x14, DMAV) // DMA

    // Enter Configuration Mode
    ENCG ()

    // Set Logical Device to select EPP
    Store (SIO_LPT, CR07)

    // Set the IO Base Address
    Store (IOLO, CR61)
    Store (IOHI, CR60)

    FindSetRightBit(IRQL,CR70)// Set IRQ
    If (LNotEqual (IRQL,Zero)) {
      Decrement  (CR70)
    }

    If (LEqual(DMAV,Zero)) {
      Store (0x04, CR74)
    } Else {
       FindSetRightBit(DMAV,CR74) // Find DMA - 1 based
       Decrement (CR74)
    }// Set DMA

    // Activate the Device
    Store (0x01, CR30)

    // Exit Configuration Mode
    EXCG()
    
  } // end _SRS Method
} // End ECP Device

