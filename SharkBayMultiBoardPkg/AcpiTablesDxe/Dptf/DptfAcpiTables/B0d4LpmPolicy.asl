/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  B0d4LpmPolicy.asl

Abstract:

  Intel ACPI Reference Code for Intel Low Power Mode

--*/


Scope(\_SB.IETM)
{
    Name (LPSP, Package ()
    {
    //
    // LPM Policy
    //
      ToUUID("B9455B06-7949-40c6-ABF2-363A70C8706C")
    })

    // CLPM (Current Low Power Mode Setting)
    //
    // This object evaluates to an integer that indicates the current platform desired LPM mode setting.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    0 - Disable LPM
    //    1 - Enable LPM
    //    2 - Application Specific LPM
    //    3 - Use the LPM setting as requested by the OS level interfaces either through the OS Power Plan settings or 
    //        through the LPM API exposed to OS software.
    //
    Method (CLPM)
    {
        If(LEqual(\_SB.PCI0.B0D4.LPMS,0)){ // check LPM support in CPU
          Return (0)
        }
        Return (LPMV) // Update LPMV based on current platform desire.
    } // End of CLPM object

}//end Scope(\_SB.IETM)

