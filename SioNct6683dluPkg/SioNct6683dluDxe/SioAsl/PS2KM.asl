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

Device(PS2K) {  // PS2 Keyboard w/ PS2 mouse support

  //
  // PnP Device ID
  //
  Name (_HID, EISAID("PNP0303"))
  Name (_CID, EISAID("PNP030B"))    // Compatible ID, PC/AT Enhanced Keyboard 101/102
  Name (_UID, "SModuleK")

  Method (_STA, 0) {
    //
    // Depend on device existence through MailBox to enable or disable this device
    // 
    if (EDVC) {             
      Return (0x0F)  
    } else {
      Return (0x00)
    }
  }

  Name (_CRS, ResourceTemplate() {
    IO (Decode16, 0x60, 0x60, 0x01, 0x01)
    IO (Decode16, 0x64, 0x64, 0x01, 0x01)
    IRQNoFlags(){1}
  })
} // end of Device PS2K

Device(PS2M) {  // PS/2 type mouse port

  Name (_HID, EISAID("PNP0F03"))  // Hardware Device ID - Microsoft mouse
  Name (_CID, EISAID("PNP0F13"))  // Compatible ID
  Name (_UID, "SModuleM")

  Method (_STA, 0) {
    //
    // Depend on device existence through MailBox to enable or disable this device
    // 
    if (EDVC) {             
      Return (0x0F)  
    } else {
      Return (0x00)
    }
  }

  // If Win9x support is required, you might have to add a conditional CRS method that
  // declares resources for port 60/64 if PS2-keyboard is not present.
  Name (_CRS, ResourceTemplate() {
    IO (Decode16, 0x60, 0x60, 0x01, 0x01)
    IO (Decode16, 0x64, 0x64, 0x01, 0x01)
    IRQNoFlags(){12}
  })
} // end of Device PS2M

// end of file Ps2KM.asl
