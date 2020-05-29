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


  // Define the SMSC Runtime I/O Registers and the LPC Docking I/O Register
  // as an ACPI Operating Region.  The base address for both can be found
  // in the SMSC 1007 Configuration Registers, specfically at CR30 for the
  // Runtime I/O Base and CR3B and CR3C for the LPC Docking Base.
  //
Scope(\)
{
  OperationRegion(S_IO,SystemIo,\IO2B,0x11)
  Field(S_IO, ByteAcc, NoLock, Preserve)
  {
  PMS0, 8,
  PME0, 8,
  PMS1, 8,
  PMS2, 8,
  PMS3, 8,
  PME1, 8,
  PME2, 8,
  PME3, 8,
  Offset(0x08),
  SS10, 1,
  SS11, 1,
  SS12, 1,
  SS13, 1,
  SS14, 1,
  SS15, 1,
  SS16, 1,
  SS17, 1,
  SMS2, 8,
  Offset(0x0A),
  SE10, 1,
  SE11, 1,
  SE12, 1,
  SE13, 1,
  SE14, 1,
  SE15, 1,
  SE16, 1,
  SE17, 1,
  SME2, 8,
  Offset(0x0C),
  RT10, 1,  // GP10 = 1 = SMSC COM Port On, 0 = Off, in Wake Mode.
  RT11, 1,  // GP11
  RT12, 1,  // GP12
  RT13, 1,  // GP13
  RT14, 1,  // GP14
  RT15, 1,  // GP15
  RT16, 1,  // GP16
  RT17, 1,  // GP17
  Offset(0x0E),
  RT30, 1,  // GP30
  RT31, 1,  // GP31
  RT32, 1,  // GP32
  RT33, 1,  // GP33
  RT34, 1,  // GP34
  RT35, 1,  // GP35
  RT36, 1,  // GP36
  RT37, 1,  // GP37
  Offset(0x10),
  DLPC, 1,  // DLPC Switch
  CK33, 1,  // 33MHz Clock
  CK14, 1 // 14MHz Clock
  }
}

Device(SM17)
{
  Name(_HID,EISAID("PNP0A05"))

  Name(_UID,2)

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // present on the Docking Board.

    If(SMSC)
    {
      Return(0x000F)
    }

    Return(0x0000)
  }

  // Define the SM17 Reduced SIO Index/Data Registers as an ACPI
  // Operating Region.  These registers will be used communicate
  // to the SIO.

  OperationRegion(SM17,SystemIO,\SP1O,0x02)
  Field(SM17, ByteAcc, Lock, Preserve)
  {
    INDX, 8,
    DATA, 8
  }

  // Use ACPI Defined IndexField so consecutive Index/Data I/Os are
  // assured to be uninterrupted.

  IndexField(INDX,DATA,ByteAcc,Lock,Preserve)
  {
    Offset(0x02),
    CR02, 8,  // UART Power Management
                Offset(0x0C),
                CR0C, 8,  // UARTMODE
    Offset(0x24),
    CR24, 8,  // UART 1 Base Address
    CR25, 8,  // UART 2 Base Address
    Offset(0x28),
    CR28, 8,  // UART IRQ Select
    Offset(0x2B),
        CR2B, 8,  //SCE (FIR) Base Address
        CR2C, 8,  //SCE (FIR) DMA Select
    Offset(0x3A),
    CR3A, 8,  // LPC Docking Activate
    CR3B, 8,  // LPC Docking Base (High)
    CR3C, 8,  // LPC Docking Base (Low)
    Offset(0x55),
    CR55, 8,  // Configuration Enable Key
    Offset(0xAA),
    CRAA, 8 // Configuration Disable Key
  }

  Include("SMSC1007_COM.ASL")
}
