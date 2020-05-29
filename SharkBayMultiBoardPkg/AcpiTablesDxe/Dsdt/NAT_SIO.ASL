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


Device(N393)
{
  Name(_HID,EISAID("PNP0A05"))

  Name(_UID,1)

  // Status Method for the National SIO.

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // present on the Docking Board.

    If(NATP)
    {
      Return(0x000F)
    }

    Return(0x0000)
  }

  // Define the SIO Index/Data Registers as an ACPI Operating
  // Region.  These registers will be used communicate to the SIO.

  OperationRegion(N393, SystemIO, 0x02E, 0x02)
  Field(N393, ByteAcc, Lock, Preserve)
  {
    INDX, 8,
    DATA, 8
  }

  // Use ACPI Defined IndexField so consecutive Index/Data I/Os are
  // assured to be uninterrupted.

  IndexField(INDX, DATA, ByteAcc, Lock, Preserve)
  {
    Offset(0x07), // Logical Device Number.
    R07H, 8,
    Offset(0x20), // SIO Configuration and ID.
    R20H, 8,
    R21H, 8,
    R22H, 8,
    R23H, 8,
    R24H, 8,
    R25H, 8,
    R26H, 8,
    R27H, 8,
    R28H, 8,
    R29H, 8,
    R2AH, 8,
    Offset(0x30), // Logical Device Activate.
    R30H, 8,
    Offset(0x60), // I/O Space Configuration.
    R60H, 8,
    R61H, 8,
    Offset(0x70), // Interrupt Configuration.
    R70H, 8,
    R71H, 8,
    Offset(0x74), // DMA Configuration.
    R74H, 8,
    R75H, 8,
    Offset(0xF0), // Special Logical Device Configuration.
    RF0H, 8,
    RF1H, 8
  }

  Include ("NAT_COM.ASL")
  Include ("NAT_LPT.ASL")
}
