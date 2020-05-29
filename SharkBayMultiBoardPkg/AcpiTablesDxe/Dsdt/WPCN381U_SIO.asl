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


Device(WPCN)
{
  Name(_HID,EISAID("PNP0A05"))

  Name(_UID,3)

  // Status Method for the National SIO.

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // present on the Docking Board.

    If(W381)
    {
      Return(0x000F)
    }

    Return(0x0000)
  }

  // Define the SIO Index/Data Registers as an ACPI Operating
  // Region.  These registers will be used communicate to the SIO.

  OperationRegion(WPCN, SystemIO, 0x02E, 0x02)
  Field(WPCN, ByteAcc, Lock, Preserve)
  {
    INDX, 8,
    DATA, 8
  }

  // Use ACPI Defined IndexField so consecutive Index/Data I/Os are
  // assured to be uninterrupted.

  IndexField(INDX, DATA, ByteAcc, Lock, Preserve)
  {
    Offset(0x07), // Logical Device Number.
    WR07, 8,
    Offset(0x20), // SIO Configuration and ID.
    WR20, 8,
    WR21, 8,
    WR22, 8,
    WR23, 8,
    WR24, 8,
    WR25, 8,
    WR26, 8,
    WR27, 8,
    WR28, 8,
    WR29, 8,
    WR2A, 8,
    Offset(0x30), // Logical Device Activate.
    WR30, 8,
    Offset(0x60), // I/O Space Configuration.
    WR60, 8,
    WR61, 8,
    WR62, 8,
    WR63, 8,
    Offset(0x70), // Interrupt Configuration.
    WR70, 8,
    WR71, 8,
    Offset(0x74), // DMA Configuration.
    WR74, 8,
    WR75, 8,
    Offset(0xF0), // Special Logical Device Configuration.
    WRF0, 8,
    WRF1, 8
  }

    Include ("WPCN381U_COM.asl")
    Include ("WPCN381U_LPT.ASL")
}
