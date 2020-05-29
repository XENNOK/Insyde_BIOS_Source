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

//[-start-121211-IB05960207-modify]//
Scope(\_SB.PCI0.SAT0) {
//[-start-130508-IB04770282-modify]//
  Scope(PRT2) {
//     Name(_ADR,0x0002FFFF)  // Port 2
//[-end-130508-IB04770282-modify]//
    //
    // _DSM Device Specific Method supporting SATA ZPODD function
    //
    // Arg0: UUID Unique function identifier
    // Arg1: Integer Revision Level
    // Arg2: Integer Function Index
    // Arg3: Package Parameters
    //
    Method (_DSM, 4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (LEqual(Arg0, ToUUID ("bdfaef30-aebb-11de-8a39-0800200c9a66"))) {
        //
        // Switch by function index
        //
        Switch (ToInteger(Arg2)) {
          //
          // Standard query - A bitmask of functions supported
          // Supports function 0-5
          //
          Case (0) {
            switch(ToInteger(Arg1)) {
              case(1) {
                  //
                  // Return - 0 (No Functions supported) for Desktop Platforms
                  //
                   If(LEqual(PFLV,FDTP)) { 
                      Return (Buffer () {0})
                   }
                   Return (Buffer () {0xF})
              }
              Default {Return (Buffer () {0})}
            }
          }
          //
          // Enable ZPODD feature
          //
          Case (1) {
            //
            // Enable Power ON/OFF
            //
            Return (1)
          }
          //
          // Power Off Device
          //
          Case (2) {
            Store(0,GPE3)   // Disable SCI on GPIO3 (PMBASE+0x28 - BIT19)
            //
            // Read value of the GPI3 here i.e. GPI3
            // If the GPI3 is high, , GP_INV should be set to trigger low 
            // if GPI3 is Low, GP_INV should be set to trigger high 
            //
            If(LEqual(And(\GL00, 0x08),0x08)) { // GPI3 is high
              Or(\GIV0, 0x08, \GIV0) //GP_INV should be set to trigger low 
            } Else {
              And(\GIV0, 0xF7, \GIV0) //GP_INV should be set to trigger high
            }
            //
            // Drive GPIO68 to low to power off device.
            //
            And(\GL08, 0xEF, \GL08)
            Sleep(200)
            Store(1,GPS3)   // Clear Status SCI on GPIO3 (PMBASE+0x22 - BIT19)
            Store(1,GPE3)   // Enable SCI on GPIO3 (PMBASE+0x28 - BIT19)
            P8XH (4, 0x03, 0)
            P8XH (4, 0x03, 1)
            Return (1)
          }
          //
          // Power ON Device
          //
          Case (3) {
            //
            // Drive GPIO68 to High to power on device.
            // Disable SCI on GPIO3 (PMBASE+0x28 - BIT19)
            //
            Store(0,GPE3)
            // Clear Status SCI on GPIO3 (PMBASE+0x22 - BIT19)
            Store(1,GPS3)
            Or(\GL08, 0x10, \GL08)
            P8XH (4, 0x05, 0)
            P8XH (4, 0x05, 1)
            Return (1)
          }
          Default {
            Return (0)
          }
        }
      } Else {
        Return (0)
      }
    }
  }
}

//
// GPE Event Handler
//
Scope(\_GPE) {
  //
  // GPI03 = SATA_ODD_DA 
  //
  Method(_L13) {
    //
    // Do nothing if Desktop platform
    //
    If(LEqual(PFLV,FDTP)) {
      Return ()
    }
    //
    // Power on drive, disable SCI on GPI
    // Disable SCI on GPIO3 (PMBASE+0x28 - BIT19)
    //
    Store(0,GPE3)
    //
    // Drive GPIO68 to High to power on device.
    //
    Or(\GL08, 0x10, \GL08)
    //
    // Notify the OSPM
    //
    Notify(\_SB.PCI0.SAT0, 0x82)
    Return ()
  }
}
//[-end-121211-IB05960207-modify]//
