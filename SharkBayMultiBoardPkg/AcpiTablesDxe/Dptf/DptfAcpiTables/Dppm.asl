/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  Dppm.asl

Abstract:

  Intel ACPI Reference Code for Intel Dynamic Power Performance Management

--*/


Scope(\_SB.IETM)
{

    //
    // DPPM Passive Policy
    //
    Name (DPSP, Package ()
    {
      ToUUID("42A441D6-AE6A-462B-A84B-4A8CE79027D3"),
    })

    //
    // DPPM Active Policy
    //
    Name (DASP, Package ()
    {
      ToUUID("3A95C389-E4B8-4629-A526-C52C88626BAE"),
    })

    //
    // DPPM Crtical Policy
    //
    Name (DCSP, Package ()
    {
      ToUUID("97C68AE7-15FA-499c-B8C9-5DA81D606E0A"),
    })

    //
    // DPPM Cooling Mode Policy
    //
    Name (DMSP, Package ()
    {
      ToUUID("16CAF1B7-DD38-40ed-B1C1-1B8A1913D531")
    })
} // End Scope(\_SB.IETM)
