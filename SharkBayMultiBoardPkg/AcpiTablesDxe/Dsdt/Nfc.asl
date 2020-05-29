                                                                                                          /**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Ivy Bridge          *;
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

//
// NFC modules definitions.
//

scope (\)
{
  //
  // Magnetics Peak NFC module
  //
  Device(NFC1)
  {
    Name(_HID, EISAID("SKTD000"))

    Method(_STA)
    {
      If(LEqual(NFCE, 3)) 
      {
        Return(0x0F)     // NFC enabled, show it
      }
      Else
      {
        Return(0x00)    // NFC disabled, hide it
      }
    }
  }

  //
  // NXP NFC module
  //
  Device(NFC2)
  {
    Name(_HID, EISAID("NXP5442"))

    Method(_STA)
    {
      If(LEqual(NFCE, 2)) 
      {
        Return(0x0F)     // NFC enabled, show it
      }
      Else
      {
        Return(0x00)    // NFC disabled, hide it
      }
    }
  }

  //
  // Magnetics Peak 2 NFC module
  //
  Device(NFC3)
  {
    Name(_HID, EISAID("ICV0A12"))

    Method(_STA)
    {
      If(LEqual(NFCE, 1)) 
      {
        Return(0x0F)     // NFC enabled, show it
      }
      Else
      {
        Return(0x00)    // NFC disabled, hide it
      }
    }
  }
}

