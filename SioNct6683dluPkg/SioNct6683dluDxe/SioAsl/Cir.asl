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

#define SIO_CIR    GLDN

Device(CIRD) {  // CIR device

  Name (_HID, EISAID("PNP0510"))  // PnP Device ID
  Name(_UID, "SModuleCir")      // Generic ID for IrDA

  //*****************************************************
  // Method _STA:  Return Status
  //*****************************************************
  Method (_STA, 0, NotSerialized) { // Return Status of the CIR
    
    // Enter Configuration Mode
    ENCG ()

    // Select the CIR
    Store (SIO_CIR, CR07)

    // Default setting: no device
    Store (0x00, Local0)      

    //
    // Depend on SCU value through MailBox to enable or disable this device
    // 
    if (GDVC) {             
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
  } // end of Method _STA    


  //*****************************************************
  //  Method _DIS:  Disable the device
  //*****************************************************
  Method (_DIS, 0, NotSerialized) { // Disable Method
      
    // Enter Configuration Mode
    ENCG ()

    // Set Logical Device to select the CIR
    Store (SIO_CIR, CR07)

    // Set the Activate Register to zero to disable device
    Store (0x00, CR30)

    // Exit Configuration Mode
    EXCG ()
  }


  //*****************************************************
  //  Method _CRS:  Return Current Resource Settings
  //*****************************************************
  Method (_CRS, 0, NotSerialized) {
  
    Name (BUF0, ResourceTemplate() {
      IO (Decode16, 0x320, 0x320, 0x08, 0x08)
      IRQ (Level, ActiveLow, Shared) {11}
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
    CreateWordField (BUF0, 0x09, IRQL) // IRQ

    // Enter Configuration Mode
    ENCG ()

    // Select the CIR
    Store (SIO_CIR, CR07)

    //
    // Get the IO setting from the chip, and copy it
    // to both the min & max for the IO descriptor.
    //

    // Low Bytes:
    Store (CR61, IOLO)    // min.
    Store (CR61, IORL)    // max.

    // High Bytes:
    Store (CR60, IOHI)   // min.
    Store (CR60, IORH)   // max.

    //
    // Get the IRQ setting from the chip, and shift
    // it into the Descriptor byte.
    //
    Store (0x01, Local0)
    ShiftLeft (Local0, CR70, IRQL)

    // Exit Configuration Mode
    EXCG ()

    Return(BUF0) // return the result

  } // end _CRS Method


  //*****************************************************
  //  Method _PRS:  Return Possible Resource Settings
  //*****************************************************
  Method(_PRS,0) {   // Possible Resource    
  
    Name(BUF0,ResourceTemplate() {
      // Good configuration for Legacy O/S and sub-optional for ACPI O/S
      StartDependentFn(0,2)
      {
       IO(Decode16,0x320,0x320,0x01,0x08)
       IRQ (Level, ActiveLow, Shared) {11}
       }
  
      // Good configuration for Legacy O/S and sub-optional for ACPI O/S
      StartDependentFn(0,2)
      {
       IO(Decode16,0x2F8,0x2F8,0x01,0x08)
       IRQ (Level, ActiveLow, Shared) {3}
       }
   
      // Good configuration for ACPI and sub-optional Legacy O/S
  
      EndDependentFn()
      })
      
    Return(BUF0)  // Return Buf0
  }  // End _PRS


  //*****************************************************
  //  Method _SRS:  Set Resource Setting
  //*****************************************************
  Method (_SRS, 1, NotSerialized) {    // Set Resource Settings
    // ARG0 is PnP Resource String to set

    //
    // Define offsets into descriptor to match value
    // returned by descriptors in _PRS method
    //
    CreateByteField (Arg0, 0x02, IOLO) // IO Port Low
    CreateByteField (Arg0, 0x03, IOHI) // IO Port Hi
    CreateWordField (Arg0, 0x09, IRQL) // IRQ

    // Enter Configuration Mode
    ENCG ()

    // Set Logical Device to select CIR
    Store(SIO_CIR, CR07)

    // Set the IO Base Address
    Store(IOLO, CR61)
    Store(IOHI, CR60)

    FindSetRightBit (IRQL, CR70)// Set IRQ
    If (LNotEqual(IRQL, Zero)) {
      Decrement(CR70)
    }
    // Activate the Device
    Store (0x01, CR30)

    // Exit Configuration Mode
    EXCG ()

  } // end _SRS Method
} // end of Device CIR

