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


Device(SM10)
{
  Name(_HID,EISAID("PNP0A05"))

  Name(_UID,4)

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // present on the Docking Board.

    If(SMC1)
    {
      Return(0x000F)
    }

    Return(0x0000)
  }

  // Define the SMSC1000 Reduced SIO Index/Data Registers as an ACPI
  // Operating Region.  These registers will be used communicate
  // to the SIO.

  OperationRegion(SM10,SystemIO,\SP2O,0x02)
  Field(SM10, ByteAcc, Lock, Preserve)
  {
    INDX, 8,
    DATA, 8
  }

  // Use ACPI Defined IndexField so consecutive Index/Data I/Os are
  // assured to be uninterrupted.

  IndexField(INDX,DATA,ByteAcc,Lock,Preserve)
  {
        CR00,8,
        CR01,8,
    Offset(0x02),
    CR02, 8,  // UART Power Management
          Offset(0x0C),
    CR0C, 8,  // UARTMODE
    Offset(0x23),
    CR23,8,
    Offset(0x24),
    CR24, 8,  // UART 1 Base Address
    CR25, 8,  // UART 2 Base Address
    Offset(0x27),
    CR27, 8,
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
  Include("SMSC1000_COM.asl")
  Include("SMSC1000_LPT.asl")
}